// #include <cstdint>
// #include <iostream>
// #define KAHYPAR_ATTRIBUTE_FALLTHROUGH __attribute__ ((fallthrough))
// #include <vector>

// template <typename Key>
// class MurmurHash {
//  public:
//   using HashValue = uint64_t;

//   explicit MurmurHash(const uint32_t seed = 0) :
//     _seed(seed)
//   { }

//   void reset(const uint32_t seed) {
//     _seed = seed;
//   }

//   inline HashValue operator() (const Key& key) const {
//     return hash(reinterpret_cast<const void*>(&key), sizeof(key), _seed);
//   }

//  private:
//   inline uint64_t hash(const void* key, const uint32_t len, const uint32_t seed) const {
//     const uint64_t m = 0xc6a4a7935bd1e995;
//     const int r = 47;

//     uint64_t h = seed ^ (len * m);

//     const uint64_t* data = (const uint64_t*)key;
//     const uint64_t* end = data + (len / 8);

//     while (data != end) {
//       uint64_t k = *data++;

//       k *= m;
//       k ^= k >> r;
//       k *= m;

//       h ^= k;
//       h *= m;
//     }

//     const unsigned char* data2 = (const unsigned char*)data;

//     switch (len & 7) {
//       case 7:
//         h ^= uint64_t(data2[6]) << 48;
//         KAHYPAR_ATTRIBUTE_FALLTHROUGH;
//       case 6:
//         h ^= uint64_t(data2[5]) << 40;
//         KAHYPAR_ATTRIBUTE_FALLTHROUGH;
//       case 5:
//         h ^= uint64_t(data2[4]) << 32;
//         KAHYPAR_ATTRIBUTE_FALLTHROUGH;
//       case 4:
//         h ^= uint64_t(data2[3]) << 24;
//         KAHYPAR_ATTRIBUTE_FALLTHROUGH;
//       case 3:
//         h ^= uint64_t(data2[2]) << 16;
//         KAHYPAR_ATTRIBUTE_FALLTHROUGH;
//       case 2:
//         h ^= uint64_t(data2[1]) << 8;
//         KAHYPAR_ATTRIBUTE_FALLTHROUGH;
//       case 1: h ^= uint64_t(data2[0]);
//         h *= m;
//     }

//     h ^= h >> r;
//     h *= m;
//     h ^= h >> r;

//     return h;
//   }

//   uint32_t _seed;
// };
//   HashValue combinedHash(const Hypergraph& graph, const VertexId vertex_id) const {
//     ALWAYS_ASSERT(getHashNum() > 0, "The number of hashes should be greater than zero");
//     HashValue val = 0;
//     for (uint32_t hash_num = 0; hash_num < _hash_func_vector.getHashNum(); ++hash_num) {
//       val ^= minHash(_hash_func_vector[hash_num], graph.incidentEdges(vertex_id));
//     }
//     return val;
//   }
// int main(){
//     using HashFunc = MurmurHash<uint32_t>;
//     using HashValue = typename HashFunc::HashValue;
//     HashFunc hash(8);
//     HashFunc hash1;
//     HashFunc hash2;
//     HashValue a=hash(3);
//     HashValue b=hash(4);
//     std::vector<HashFunc> _hash_functions;

// }
