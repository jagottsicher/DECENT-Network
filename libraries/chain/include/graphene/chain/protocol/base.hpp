/* (c) 2016, 2017 DECENT Services. For details refers to LICENSE.txt */
/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#pragma once

#include <graphene/chain/protocol/authority.hpp>
#include <fc/static_variant.hpp>
#include <fc/time.hpp>

namespace graphene { namespace chain {

   /**
    *  @defgroup operations Operations
    *  @ingroup transactions Transactions
    *  @brief A set of valid comands for mutating the globally shared state.
    *
    *  An operation can be thought of like a function that will modify the global
    *  shared state of the blockchain.  The members of each struct are like function
    *  arguments and each operation can potentially generate a return value.
    *
    *  Operations can be grouped into transactions (@ref transaction) to ensure that they occur
    *  in a particular order and that all operations apply successfully or
    *  no operations apply.
    *
    *  Each operation is a fully defined state transition and can exist in a transaction on its own.
    *
    *  @section operation_design_principles Design Principles
    *
    *  Operations have been carefully designed to include all of the information necessary to
    *  interpret them outside the context of the blockchain.   This means that information about
    *  current chain state is included in the operation even though it could be inferred from
    *  a subset of the data.   This makes the expected outcome of each operation well defined and
    *  easily understood without access to chain state.
    *
    *  @subsection balance_calculation Balance Calculation Principle
    *
    *    We have stipulated that the current account balance may be entirely calculated from
    *    just the subset of operations that are relevant to that account.  There should be
    *    no need to process the entire blockchain inorder to know your account's balance.
    *
    *  @subsection fee_calculation Explicit Fee Principle
    *
    *    Blockchain fees can change from time to time and it is important that a signed
    *    transaction explicitly agree to the fees it will be paying.  This aids with account
    *    balance updates and ensures that the sender agreed to the fee prior to making the
    *    transaction.
    *
    *  @subsection defined_authority Explicit Authority
    *
    *    Each operation shall contain enough information to know which accounts must authorize
    *    the operation.  This principle enables authority verification to occur in a centralized,
    *    optimized, and parallel manner.
    *
    *  @subsection relevancy_principle Explicit Relevant Accounts
    *
    *    Each operation contains enough information to enumerate all accounts for which the
    *    operation should apear in its account history.  This principle enables us to easily
    *    define and enforce the @ref balance_calculation. This is superset of the @ref defined_authority
    *
    *  @{
    */

   struct void_result{};
   typedef fc::static_variant<void_result,graphene::db::object_id_type> operation_result;

   template<bool VIRTUAL = false>
   struct base_operation : public std::integral_constant<bool, VIRTUAL>
   {
      template<typename T>
      share_type calculate_fee( const T& params, const fc::time_point_sec now )const
      {
         return params.fee;
      }
      void get_required_authorities( std::vector<authority>& )const{}
      void get_required_active_authorities( boost::container::flat_set<account_id_type>& )const{}
      void get_required_owner_authorities( boost::container::flat_set<account_id_type>& )const{}
      void validate()const{}

      bool is_partner_account_id(account_id_type acc_id) const { return false; }

      static uint64_t calculate_data_fee( uint64_t bytes, uint64_t price_per_kbyte );
   };

   /**
    *  For future expansion many structus include a single member of type
    *  extensions_type that can be changed when updating a protocol.  You can
    *  always add new types to a static_variant without breaking backward
    *  compatibility.
    */
   typedef fc::static_variant<void_t> future_extensions;

   /**
    *  A flat_set is used to make sure that only one extension of
    *  each type is added and that they are added in order.
    *
    *  @note static_variant compares only the type tag and not the
    *  content.
    */
   typedef boost::container::flat_set<future_extensions> extensions_type;

   ///@}

} } // graphene::chain

FC_REFLECT_TYPENAME( graphene::chain::operation_result )
FC_REFLECT_TYPENAME( graphene::chain::future_extensions )
FC_REFLECT_EMPTY( graphene::chain::void_result )

#define GRAPHENE_REFLECT_FEE( TYPE, MEMBERS ) \
namespace fc { \
   template<> void to_variant(const TYPE& o, variant& v); \
   template<> void from_variant(const variant& v, TYPE& o); \
} \
FC_REFLECT(TYPE, MEMBERS)

#define GRAPHENE_REFLECT_FEE_MEMBER( r, o, MEMBER ) \
   if(o.MEMBER.has_value()) mvo.set(BOOST_PP_STRINGIZE(MEMBER), o.MEMBER.instance);

#define GRAPHENE_REFLECT_FEE_IMPL( TYPE, FEE, MEMBERS ) \
namespace fc { \
   template<> void to_variant(const TYPE& o, variant& v) { \
      mutable_variant_object mvo(BOOST_PP_STRINGIZE(FEE), o.FEE); \
      BOOST_PP_SEQ_FOR_EACH( GRAPHENE_REFLECT_FEE_MEMBER, o, MEMBERS ) \
      v = std::move(mvo); \
   } \
   template<> void from_variant(const variant& v, TYPE& o) { \
      const variant_object& vo = v.get_object(); \
      reflector<TYPE>::visit(from_variant_visitor<TYPE>(vo, o)); \
   } \
}
