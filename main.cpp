#include <cstddef>
#include <cstdlib>
#include <string>

#include "TreeMap.h"
#include "HashMap.h"
namespace
{



void performTest(int map_size)
 {

    aisdi::TreeMap<int, int> tmap;
    aisdi::HashMap<int, int> hmap((map_size / 2) + 1);
    clock_t start, runtime;

    int key;
    start = clock();
    int i;
    for (i = 0; i < map_size; i++)
    {
        key = std::rand()% (map_size);
        tmap[key] = i;
    }
    runtime = clock() - start;
    std::cout << "TreeMap add "<<map_size<< "pairs: " << (double)runtime / CLOCKS_PER_SEC << " seconds" << std::endl;

    start = clock();
    for (int i = 0; i < map_size; i++)
    {
        key = std::rand()% (map_size);
        hmap[key] = i;
    }
    runtime = clock() - start;
    std::cout << "HahshMap add "<<map_size<< "pairs: " << (double)runtime / CLOCKS_PER_SEC << " seconds" << std::endl;


    start = clock();
    for (int i=map_size; i>0;i--)
        tmap.find(i);
    runtime = clock() - start;
    std::cout << "TreeMap find "<<map_size<< "keys: " << (double)runtime / CLOCKS_PER_SEC << " seconds" << std::endl;

    start = clock();
    for (int i=map_size; i>0;i--)
        hmap.find(i);
    runtime = clock() - start;
    std::cout << "HashMap find "<<map_size<< "keys: " << (double)runtime / CLOCKS_PER_SEC << " seconds" << std::endl;

    start = clock();
    for(auto it = tmap.begin(); it != tmap.end(); ++it);

    runtime = clock() - start;
    std::cout << "TreeMap iterate : " << (double)runtime / CLOCKS_PER_SEC << " seconds" << std::endl;

    start = clock();
    for(auto it = hmap.begin(); it != hmap.end(); ++it);
    runtime = clock() - start;
    std::cout << "HashMap iterate : " << (double)runtime / CLOCKS_PER_SEC << " seconds" << std::endl;


}


} // namespace

int main(int argc, char** argv)
{
  const std::size_t repeatCount = argc > 1 ? std::atoll(argv[1]) : 10000;
  for (std::size_t i = 1; i < repeatCount; ++i)
    performTest(100000*i);
  return 0;
}
