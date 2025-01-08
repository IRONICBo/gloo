#include <iostream>
#include <memory>
#include <vector>
#include <chrono>

#include "gloo/allreduce_ring.h"
#include "gloo/rendezvous/context.h"
#include "gloo/rendezvous/file_store.h"
#include "gloo/rendezvous/prefix_store.h"
#include "gloo/transport/tcp/device.h"

constexpr size_t BLOCK_SIZE = 1 * 1024 * 1024;

int main(void) {
  if (getenv("PREFIX") == nullptr || getenv("SIZE") == nullptr ||
      getenv("RANK") == nullptr || getenv("BLOCK_SIZE") == nullptr) {
    std::cerr << "Please set environment variables PREFIX, SIZE, BLOCK_SIZE, and RANK."
              << std::endl;
    return 1;
  }

  auto num_block_size = getenv("BLOCK_SIZE");
  auto block_size = BLOCK_SIZE;
  block_size = atoi(num_block_size) * BLOCK_SIZE;

  gloo::transport::tcp::attr attr;
  attr.iface = "lo";  // Loopback interface
  attr.ai_family = AF_UNSPEC;
  auto dev = gloo::transport::tcp::CreateDevice(attr);


  auto fileStore = gloo::rendezvous::FileStore("/home/lvbo/project/gloo_bench/gloo/build/tmp");
  std::string prefix = getenv("PREFIX");
  auto prefixStore = gloo::rendezvous::PrefixStore(prefix, fileStore);

  const int rank = atoi(getenv("RANK"));
  const int size = atoi(getenv("SIZE"));
  auto context = std::make_shared<gloo::rendezvous::Context>(rank, size);
  context->connectFullMesh(prefixStore, dev);

  std::vector<int> sendBuffer(block_size / sizeof(int), rank);
  std::vector<int> recvBuffer(block_size / sizeof(int), 0);

  if (rank == 0) {
    std::cout << "Testing Gloo AllreduceRing with " <<  num_block_size << "MB blocks..." << std::endl;
  }

  const int iterations = 100;
  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < iterations; i++) {
    auto allreduce = std::make_shared<gloo::AllreduceRing<int>>(
        context, std::vector<int*>{sendBuffer.data()}, sendBuffer.size());
    allreduce->run();
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end - start;

  double totalDataMB = (block_size * iterations * size) / (1024.0 * 1024.0);
  double throughput = totalDataMB / duration.count();

  if (rank == 0) {
    std::cout << "Total data transferred: " << totalDataMB << " MB" << std::endl;
    std::cout << "Total time: " << duration.count() << " seconds" << std::endl;
    std::cout << "Throughput: " << throughput << " MB/s" << std::endl;
  }

  return 0;
}
