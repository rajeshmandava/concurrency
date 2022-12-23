#include <iostream>
#include <condition_variable>
#include <thread>
#include <string>
#include <mutex>

std::mutex m;
std::condition_variable cv;
std::string data;
bool ready= false;
bool processed = false;


void worker_thread()
{
  std::unique_lock lk(m);
  cv.wait(lk, []{return ready;});

  std::cout<<"Worker thread is processing data"<<std::endl;
  data+=" after processing\n";

  processed = true;
  std::cout<<"[worker_thread] processing data completed"<<std::endl;

  lk.unlock();
  cv.notify_one();
}

int main()
{
  std::thread t(worker_thread);
  data = "example data";
  {
    std::lock_guard lk(m);
    ready = true;
    std::cout<<"[main] signals data ready for processing"<< std::endl;
  }
  cv.notify_one();
  {
    std::unique_lock lk(m);
    cv.wait(lk,[]{return processed;});
  }
  std::cout<< "[main] Back to main data:"<<data<< std::endl;
  t.join();
}