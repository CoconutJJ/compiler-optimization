#pragma once // NOLINT(llvm-header-guard)

#include "DFA/Flow/Framework.h"
#include <vector>

namespace dfa
{

template <typename TValue>
struct MeetOpBase {
        using DomainVal_t = std::vector<TValue>;
        /// @brief Apply the meet operator using two operands.
        /// @param LHS
        /// @param RHS
        /// @return
        virtual DomainVal_t operator() (const DomainVal_t &LHS, const DomainVal_t &RHS) const = 0;
        /// @brief Return a domain value that represents the top element, used when
        ///        doing the initialization.
        /// @param DomainSize
        /// @return
        virtual DomainVal_t top (const std::size_t DomainSize) const = 0;
};

template <typename TValue>
struct Intersect final : MeetOpBase<TValue> {
        using DomainVal_t = typename MeetOpBase<TValue>::DomainVal_t;

        DomainVal_t operator() (const DomainVal_t &LHS, const DomainVal_t &RHS) const final
        {
                DomainVal_t intersection (LHS.size ());

                for (size_t i = 0; i < LHS.size (); i++) {
                        intersection[i] = LHS[i] & RHS[i];
                }

                return intersection;
        }
        DomainVal_t top (const std::size_t DomainSize) const final
        {
                return DomainVal_t (DomainSize, true);
        }
};

template <typename TValue>
struct Union final : MeetOpBase<TValue> {
        using DomainVal_t = typename MeetOpBase<TValue>::DomainVal_t;

        DomainVal_t operator() (const DomainVal_t &LHS, const DomainVal_t &RHS) const final
        {
                DomainVal_t intersection (LHS.size ());

                for (size_t i = 0; i < LHS.size (); i++) {
                        intersection[i] = LHS[i] | RHS[i];
                }

                return intersection;
        }
        DomainVal_t top (const std::size_t DomainSize) const final
        {
                return DomainVal_t (DomainSize, Bool{ .Value = false });
        }
};

} // namespace dfa
