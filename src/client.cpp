
#include <string.h>

#include <condition_variable>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>

// Socket.IO C++ Clientのヘッダファイル
#include <sio_client.h>

class SampleClient {
 public:
  // Socket.IOのインスタンス
  sio::client client;
  sio::socket::ptr socket;

  // sioのスレッドとメインスレッドの同期をとるためのmutex類
  std::mutex sio_mutex;
  std::condition_variable_any sio_cond;
  // sioのメッセージを貯めるためのキュー
  std::queue<sio::message::ptr> sio_queue;

  bool is_connected = false;

  // 切断時に呼び出されるイベントリスナ
  void on_close() {
    std::cout << "切断しました。" << std::endl;
    exit(EXIT_FAILURE);
  }

  //エラー発生時に呼び出されるイベントリスナ
  void on_fail() {
    std::cout << "エラーがありました。" << std::endl;
    exit(EXIT_FAILURE);
  }

  // 接続時に呼び出されるイベントリスナ
  void on_open() {
    std::cout << "接続しました。" << std::endl;
    std::unique_lock<std::mutex> lock(sio_mutex);
    is_connected = true;
    // 接続処理が終わったのち、待っているメインスレッドを起こす
    sio_cond.notify_all();
  }

  // "run"コマンドのイベントリスナ
  void on_run(sio::event& e) {
    std::unique_lock<std::mutex> lock(sio_mutex);
    sio_queue.push(e.get_message());
    // イベントをキューに登録し、待っているメインスレッドを起こす
    sio_cond.notify_all();
  }

  // メイン処理
  void run(const std::string& url, const std::string& name) {
    // 接続およびエラーイベントのリスナを設定する
    client.set_close_listener(std::bind(&SampleClient::on_close, this));
    client.set_fail_listener(std::bind(&SampleClient::on_fail, this));
    client.set_open_listener(std::bind(&SampleClient::on_open, this));

    // 接続要求を出す
    client.connect(url);
    {
      // 別スレッドで動く接続処理が終わるまで待つ
      std::unique_lock<std::mutex> lock(sio_mutex);
      if (!is_connected) {
        sio_cond.wait(sio_mutex);
      }
    }

    // "run"コマンドのリスナを登録する
    socket = client.socket();
    socket->on("run", std::bind(&SampleClient::on_run, this, std::placeholders::_1));

    {
      sio::message::ptr send_data(sio::object_message::create());
      std::map<std::string, sio::message::ptr>& map = send_data->get_map();

      // objectのメンバ、typeとnameを設定する
      map.insert(std::make_pair("type", sio::string_message::create("native")));
      map.insert(std::make_pair("name", sio::string_message::create(name)));

      // joinコマンドをサーバに送る
      socket->emit("join", send_data);
    }

    while (true) {
      // イベントキューが空の場合、キューが補充されるまで待つ
      std::unique_lock<std::mutex> lock(sio_mutex);
      while (sio_queue.empty()) {
        sio_cond.wait(lock);
      }

      // イベントキューから登録されたデータを取り出す
      sio::message::ptr recv_data(sio_queue.front());
      std::stringstream output;
      char buf[1024];

      FILE* fp = nullptr;
      // objectのcommandメンバの値を取得する
      std::string command = recv_data->get_map().at("command")->get_string();
      std::cout << "run:" << command << std::endl;
      // commandを実行し、実行結果を文字列として取得する
      if ((fp = popen(command.c_str(), "r")) != nullptr) {
        while (!feof(fp)) {
          size_t len = fread(buf, 1, sizeof(buf), fp);
          output << std::string(buf, len);
        }
      } else {
        // エラーを検出した場合はエラーメッセージを取得する
        output << strerror(errno);
      }

      pclose(fp);

      sio::message::ptr send_data(sio::object_message::create());
      std::map<std::string, sio::message::ptr>& map = send_data->get_map();

      // コマンドの実行結果をobjectのoutputに設定する
      map.insert(std::make_pair("output", sio::string_message::create(output.str())));

      // sio::messageをサーバに送る
      socket->emit("reply", send_data);

      // 処理が終わったイベントをキューから取り除く
      sio_queue.pop();
    }
  }
};

int main(int argc, char* argv[]) {
  // 引数を１つとり、名前とする
  if (argc != 3) {
    std::cerr << "接続先と名前を引数に指定してください。" << std::endl;
    exit(EXIT_FAILURE);
  }

  SampleClient client;
  client.run(argv[1], argv[2]);

  return EXIT_SUCCESS;
}
