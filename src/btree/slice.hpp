#ifndef __BTREE_SLICE_HPP__
#define __BTREE_SLICE_HPP__

#include "store.hpp"
#include "buffer_cache/buffer_cache.hpp"
#include "serializer/translator.hpp"

class initialize_superblock_fsm_t;
struct btree_replicant_t;

class slice_store_t : public store_t {
public:
    virtual int slice_home_thread() = 0;
};


/* btree_slice_t is a thin wrapper around cache_t that handles initializing the buffer
cache for the purpose of storing a btree. There are many btree_slice_ts per
btree_key_value_store_t. */

class btree_slice_t :
    public slice_store_t,
    public home_thread_mixin_t
{
public:
    // Blocks
    static void create(translator_serializer_t *serializer,
                       mirrored_cache_config_t *dynamic_config,
                       mirrored_cache_static_config_t *static_config);

    // Blocks
    btree_slice_t(translator_serializer_t *serializer,
                  mirrored_cache_config_t *dynamic_config,
                  mirrored_cache_static_config_t *static_config);

    // Blocks
    ~btree_slice_t();

    /* store_t interface. */
    get_result_t get(store_key_t *key);
    get_result_t get_cas(store_key_t *key, castime_t castime);
    rget_result_ptr_t rget(store_key_t *start, store_key_t *end, bool left_open, bool right_open);
    set_result_t sarc(store_key_t *key, data_provider_t *data, mcflags_t flags, exptime_t exptime, castime_t castime,
                      add_policy_t add_policy, replace_policy_t replace_policy, cas_t old_cas);


    incr_decr_result_t incr_decr(incr_decr_kind_t kind, store_key_t *key, uint64_t amount, castime_t castime);
    append_prepend_result_t append_prepend(append_prepend_kind_t kind, store_key_t *key, data_provider_t *data, castime_t castime);

    delete_result_t delete_key(store_key_t *key, repli_timestamp timestamp);

    cache_t& cache() { return cache_; }

    int slice_home_thread() { return home_thread; }

private:
    cache_t cache_;

    DISABLE_COPYING(btree_slice_t);
};

#endif /* __BTREE_SLICE_HPP__ */
