#ifndef VEC_UTIL_HPP
#define VEC_UTIL_HPP

#include <algorithm>
#include <cassert>

#include "util_template.hpp"

/********************************** Matrix ************************************/

// NOTE: Matrix is defined to be row-major

template<
    typename T,
    unsigned int row_count,
    unsigned int col_count,
    typename Enable = void
>
class Matrix {};

template<typename T, unsigned int row_count, unsigned int col_count>
class Matrix<
    T,
    row_count,
    col_count,
    enable_if_t<
        std::is_arithmetic<T>::value && row_count != 0 && col_count != 0
    >
> {
private:
    template<typename A>
    struct StaticCaster {
        enable_if_t<std::is_arithmetic<A>::value, T> operator()(A a) {
            return static_cast<T>(a);
        }
    };

    T vals[row_count][col_count];

public:
    // constructors
    Matrix() {
        std::fill(
            &this->vals[0][0],
            &this->vals[0][0] + row_count * col_count,
            0);
    }

    template<
        typename A,
        unsigned int rc,
        unsigned int cc,
        enable_if_t<rc == row_count && cc == col_count>* = nullptr
    >
    Matrix(const Matrix<A, rc, cc> &rhs) {
        *this = rhs;
    }

    template<typename A, enable_if_t<std::is_arithmetic<A>::value>* = nullptr>
    Matrix(std::initializer_list<A> args) {
        assert(args.size() == row_count * col_count);
        T *v = &this->vals[0][0];
        for (const auto &arg : args) {
            *v++ = static_cast<T>(arg);
        }
    }

    // destructor
    virtual ~Matrix() {
        //
    }

    // assignment operator
    template<typename A>
    Matrix &operator=(const Matrix<A, row_count, col_count> &rhs) {
        std::transform(
            rhs.cbegin(),
            rhs.cend(),
            &this->vals[0][0],
            StaticCaster<A>());
        return *this;
    }

    // comparison operator
    template<
        typename A,
        enable_if_t<std::is_arithmetic<A>::value && col_count != 1>* = nullptr
    >
    bool operator==(const Matrix<A, row_count, col_count> &rhs) const {
        for (unsigned int r = 0; r < row_count; r++) {
            for (unsigned int c = 0; c < col_count; c++) {
                if (this->vals[r][c] != rhs[r][c]) {
                    return false;
                }
            }
        }
        return true;
    }

    template<typename A>
    bool operator!=(const Matrix<A, row_count, col_count> &rhs) const {
        return !(*this == rhs);
    }

    // accessor operator
    template<typename I, enable_if_t<std::is_integral<I>::value>* = nullptr>
    struct _row_accessor_op_t {
    private:
        T *row;

        _row_accessor_op_t() {} // disabled

    public:
        _row_accessor_op_t(T *row) : row(row) {}

        T &operator[](const I c) {
            assert(c >= 0 && c < (I) col_count);
            return this->row[c];
        }
    };

    template<typename I, enable_if_t<std::is_integral<I>::value>* = nullptr>
    struct _row_const_accessor_op_t {
    private:
        const T *row;

        _row_const_accessor_op_t() {} // disabled

    public:
        _row_const_accessor_op_t(const T *row) : row(row) {}

        const T &operator[](const I c) const {
            assert(c >= 0 && c < (I) col_count);
            return this->row[c];
        }
    };

    template<
        typename I,
        enable_if_t<std::is_integral<I>::value && col_count != 1>* = nullptr
    >
    _row_accessor_op_t<I> operator[](const I r) {
        assert(r >= 0 && r < (I) row_count);
        return _row_accessor_op_t<I>(&this->vals[r][0]);
    }

    template<
        typename I,
        enable_if_t<std::is_integral<I>::value && col_count != 1>* = nullptr
    >
    const _row_const_accessor_op_t<I> operator[](const I r) const {
        assert(r >= 0 && r < (I) row_count);
        return _row_const_accessor_op_t<I>(&this->vals[r][0]);
    }

    // scalar math operators
    template<typename A, enable_if_t<std::is_arithmetic<A>::value>* = nullptr>
    Matrix &operator*=(const A a) {
        for (unsigned int r = 0; r < row_count; r++) {
            for (unsigned int c = 0; c < col_count; c++) {
                this->vals[r][c] *= a;
            }
        }
        return *this;
    }

    template<typename A, enable_if_t<std::is_arithmetic<A>::value>* = nullptr>
    Matrix &operator/=(const A a) {
        for (unsigned int r = 0; r < row_count; r++) {
            for (unsigned int c = 0; c < col_count; c++) {
                this->vals[r][c] /= a;
            }
        }
        return *this;
    }

    template<typename A, enable_if_t<std::is_arithmetic<A>::value>* = nullptr>
    Matrix operator*(const A rhs) const {
        return Matrix(*this) *= rhs;
    }

    template<typename A, enable_if_t<std::is_arithmetic<A>::value>* = nullptr>
    friend Matrix operator*(const A lhs, Matrix<T, row_count, col_count> &rhs) {
        return Matrix(rhs) *= lhs;
    }

    template<typename A, enable_if_t<std::is_arithmetic<A>::value>* = nullptr>
    Matrix operator/(const A a) const {
        return Matrix(*this) /= a;
    }

    Matrix operator-() const {
        return *this * -1;
    }

    // Matrix algebra operators
    template<
        typename A,
        enable_if_t<std::is_arithmetic<A>::value && col_count != 1>* = nullptr
    >
    Matrix &operator+=(const Matrix<A, row_count, col_count> &rhs) {
        for (unsigned int r = 0; r < row_count; r++) {
            for (unsigned int c = 0; c < col_count; c++) {
                this->vals[r][c] += rhs[r][c];
            }
        }
        return *this;
    }

    template<
        typename A,
        enable_if_t<std::is_arithmetic<A>::value && col_count != 1>* = nullptr
    >
    Matrix &operator-=(const Matrix<A, row_count, col_count> &rhs) {
        for (unsigned int r = 0; r < row_count; r++) {
            for (unsigned int c = 0; c < col_count; c++) {
                this->vals[r][c] -= rhs[r][c];
            }
        }
        return *this;
    }

    template<typename A>
    Matrix operator+(const Matrix<A, row_count, col_count> &rhs) const {
        return Matrix(*this) += rhs;
    }

    template<typename A>
    Matrix operator-(const Matrix<A, row_count, col_count> &rhs) const {
        return Matrix(*this) -= rhs;
    }

    template<
        typename A,
        unsigned int rhs_cc,
        enable_if_t<rhs_cc != 1>* = nullptr
    >
    Matrix<A, row_count, rhs_cc> operator*(
        const Matrix<A, col_count, rhs_cc> &rhs) const
    {
        Matrix<A, row_count, rhs_cc> result;
        for (unsigned int r = 0; r < row_count; r++) {
            for (unsigned int c = 0; c < rhs_cc; c++) {
                for (unsigned int cur = 0; cur < col_count; cur++) {
                    result[r][c] += this->vals[r][cur] * rhs[cur][c];
                }
            }
        }
        return result;
    }

    // general linear algebra utils
    Matrix transpose() const {
        Matrix<T, col_count, row_count> result;
        for (unsigned int r = 0; r < row_count; r++) {
            for (unsigned int c = 0; c < col_count; c++) {
                result[r][c] = this->vals[c][r];
            }
        }
        return result;
    }

    /************************** Vector Specializations ************************/

    template<
        typename I,
        enable_if_t<std::is_integral<I>::value && col_count == 1>* = nullptr
    >
    T &operator[](const I r) {
        assert(r >= 0 && r < (I) row_count);
        return this->vals[r][0];
    }

    template<
        typename I,
        enable_if_t<std::is_integral<I>::value && col_count == 1>* = nullptr
    >
    const T &operator[](const I r) const {
        assert(r >= 0 && r < (I) row_count);
        return this->vals[r][0];
    }

    template<
        typename A,
        enable_if_t<std::is_arithmetic<A>::value && col_count == 1>* = nullptr
    >
    bool operator==(const Matrix<A, row_count, col_count> &rhs) const {
        for (unsigned int r = 0; r < row_count; r++) {
            if (this->vals[r][0] != rhs[r]) {
                return false;
            }
        }
        return true;
    }

    template<
        typename A,
        enable_if_t<std::is_arithmetic<A>::value && col_count == 1>* = nullptr
    >
    Matrix &operator+=(const Matrix<A, row_count, col_count> &rhs) {
        for (unsigned int r = 0; r < row_count; r++) {
            this->vals[r][0] += rhs[r];
        }
        return *this;
    }

    template<
        typename A,
        enable_if_t<std::is_arithmetic<A>::value && col_count == 1>* = nullptr
    >
    Matrix &operator-=(const Matrix<A, row_count, col_count> &rhs) {
        for (unsigned int r = 0; r < row_count; r++) {
            this->vals[r][0] -= rhs[r];
        }
        return *this;
    }

    template<
        typename A,
        unsigned int rhs_cc,
        enable_if_t<rhs_cc == 1>* = nullptr
    >
    Matrix<A, row_count, rhs_cc> operator*(
        const Matrix<A, col_count, rhs_cc> &rhs) const
    {
        Matrix<A, row_count, rhs_cc> result;
        for (unsigned int r = 0; r < row_count; r++) {
            for (unsigned int cur = 0; cur < col_count; cur++) {
                result[r] += this->vals[r][cur] * rhs[cur];
            }
        }
        return result;
    }

    // Vector algebra utils
    template<
        typename A,
        enable_if_t<std::is_arithmetic<A>::value && col_count == 1>* = nullptr
    >
    T dot(const Matrix<A, row_count, 1> &rhs) const {
        T dot = 0;
        for (unsigned int i = 0; i < row_count; i++) {
            dot += this->vals[i][0] * rhs[i];
        }
        return dot;
    }

    template<
        typename A,
        enable_if_t<
            std::is_arithmetic<A>::value
            && row_count == 3 && col_count == 1
        >* = nullptr
    >
    Matrix cross(const Matrix<A, 3, 1> &rhs) const {
        return Matrix({
            this->vals[1][0] * rhs[2] - this->vals[2][0] * rhs[1],
            this->vals[2][0] * rhs[0] - this->vals[0][0] * rhs[2],
            this->vals[0][0] * rhs[1] - this->vals[1][0] * rhs[0]
        });
    }

    T squaredNorm() const {
        assert(col_count == 1);
        T norm = 0;
        for (unsigned int r = 0; r < row_count; r++) {
            norm += this->vals[r][0] * this->vals[r][0];
        }
        return norm;
    }

    float norm() const {
        assert(col_count == 1);
        return sqrt(this->squaredNorm());
    }

    Matrix &normalizeInPlace() {
        assert(col_count == 1);
        return *this /= norm();
    }

    Matrix normalize() const {
        assert(col_count == 1);
        return *this / norm();
    }
};

typedef Matrix<int, 2, 2> Matrix2i;
typedef Matrix<int, 3, 3> Matrix3i;
typedef Matrix<float, 2, 2> Matrix2f;
typedef Matrix<float, 3, 3> Matrix3f;
typedef Matrix<float, 4, 4> Matrix4f;

/********************************** Vector ************************************/

template<typename T, unsigned int dim>
using Vector = Matrix<T, dim, 1>;

typedef Vector<int, 2> Vector2i;
typedef Vector<int, 3> Vector3i;
typedef Vector<float, 2> Vector2f;
typedef Vector<float, 3> Vector3f;
typedef Vector<float, 4> Vector4f;

#endif
