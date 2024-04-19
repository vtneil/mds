// #ifndef ALIGNED_MATRIX_H
// #define ALIGNED_MATRIX_H
//
// #include "container/generic.h"
// #include "utils/io.h"
// #include "memory/allocator.h"
//
// namespace container {
//     template<typename Tp>
//     class aligned_matrix_t {
//     private:
//         using row_t = std::vector<Tp>;
//         using col_t = std::vector<row_t>;
//         using const_col_t = std::vector<types::const_reference<row_t> >;
//
//         std::vector<Tp> data;
//         std::vector<size_t> row_sizes;
//         size_t row_size;
//         size_t num_rows;
//
//     public:
//         explicit constexpr aligned_matrix_t(types::const_reference<col_t> input_vector) {
//             num_rows = input_vector.size();
//             row_sizes.resize(num_rows, 0);
//             size_t largest_row_size = 0;
//
//             for (size_t i = 0; i < num_rows; ++i) {
//                 row_sizes[i] = input_vector[i].size();
//                 largest_row_size = input_vector[i].size() > largest_row_size
//                                        ? input_vector[i].size()
//                                        : largest_row_size;
//             }
//
//             // Use power of two size?
//             row_size = builtin::next_power_of_two(largest_row_size);
//
//             // Pre-allocate
//             data.resize(num_rows * row_size, Tp{});
//
//             // Copy
//             for (size_t i = 0; i < num_rows; ++i)
//                 std::memcpy(&data[i * row_size], &input_vector[i][0], sizeof(Tp) * size_at(i));
//         }
//
//         constexpr types::pointer<Tp> operator[](const size_t idx) {
//             return &data[idx * row_size];
//         }
//
//         constexpr types::pointer_to_const<Tp> operator[](const size_t idx) const {
//             return &data[idx * row_size];
//         }
//
//         constexpr types::reference<Tp> at(const size_t row, const size_t col) {
//             return data[row * row_size + col];
//         }
//
//         constexpr types::const_reference<Tp> at(const size_t row, const size_t col) const {
//             return data[row * row_size + col];
//         }
//
//         [[nodiscard]] constexpr size_t size() const {
//             return num_rows;
//         }
//
//         [[nodiscard]] constexpr size_t size_at(const size_t idx) const {
//             return row_sizes[idx];
//         }
//     };
// }
//
// #endif //ALIGNED_MATRIX_H
