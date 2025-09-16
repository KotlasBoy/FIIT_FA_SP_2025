#include <iterator>
#include <utility>
#include <vector>
#include <boost/container/static_vector.hpp>
#include <concepts>
#include <stack>
#include <map>
#include <pp_allocator.h>
#include <search_tree.h>
#include <initializer_list>
#include <logger_guardant.h>

#ifndef MP_OS_B_TREE_H
#define MP_OS_B_TREE_H

template <typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5>
class B_tree final : private logger_guardant, private compare
{
public:
    using tree_data_type = std::pair<tkey, tvalue>;
    using tree_data_type_const = std::pair<const tkey, tvalue>;
    using value_type = tree_data_type_const;

private:
    static constexpr const size_t minimum_keys_in_node = t - 1;
    static constexpr const size_t maximum_keys_in_node = 2 * t - 1;

    inline bool compare_keys(const tkey &lhs, const tkey &rhs) const;
    inline bool compare_pairs(const tree_data_type &lhs, const tree_data_type &rhs) const;

    struct btree_node
    {
        boost::container::static_vector<tree_data_type, maximum_keys_in_node + 1> _keys;
        boost::container::static_vector<btree_node *, maximum_keys_in_node + 2> _pointers; // ук на потомков
        btree_node() noexcept;
    };

public:
    void erase_recursive(btree_node *node, const tkey &k);
    void remove_from_leaf(btree_node *node, size_t key_idx);
    void remove_from_internal(btree_node *node, size_t key_idx);
    tree_data_type get_predecessor(btree_node *node, size_t key_idx);
    tree_data_type get_successor(btree_node *node, size_t key_idx);
    void fill_child(btree_node *node, size_t child_idx);
    void borrow_from_prev(btree_node *node, size_t child_idx);
    void borrow_from_next(btree_node *node, size_t child_idx);
    void merge_children(btree_node *node, size_t child_idx);
    size_t find_key_index_in_node(btree_node *node, const tkey &k) const;

    pp_allocator<value_type> _allocator;
    logger *_logger;
    btree_node *_root;
    size_t _size;

    logger *get_logger() const noexcept override;
    pp_allocator<value_type> get_allocator() const noexcept;

public:
    explicit B_tree(const compare &cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger *logger = nullptr);

    explicit B_tree(pp_allocator<value_type> alloc, const compare &comp = compare(), logger *logger = nullptr);

    template <input_iterator_for_pair<tkey, tvalue> iterator>
    explicit B_tree(iterator begin, iterator end, const compare &cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger *logger = nullptr);

    B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare &cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger *logger = nullptr);

    B_tree(const B_tree &other);

    B_tree(B_tree &&other) noexcept;

    B_tree &operator=(const B_tree &other);

    B_tree &operator=(B_tree &&other) noexcept;

    ~B_tree() noexcept override;

    class btree_iterator;
    class btree_reverse_iterator;
    class btree_const_iterator;
    class btree_const_reverse_iterator;

    class btree_iterator final
    {
        std::stack<std::pair<btree_node **, size_t>> _path;
        size_t _index;

    public:
        using value_type = tree_data_type_const;
        using reference = value_type &;
        using pointer = value_type *;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_const_iterator;
        friend class btree_const_reverse_iterator;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self &operator++();
        self operator++(int);

        self &operator--();
        self operator--(int);

        bool operator==(const self &other) const noexcept;
        bool operator!=(const self &other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_iterator(const std::stack<std::pair<btree_node **, size_t>> &path = std::stack<std::pair<btree_node **, size_t>>(), size_t index = 0);
    };

    class btree_const_iterator final
    {
        std::stack<std::pair<btree_node **, size_t>> _path;
        size_t _index;

    public:
        using value_type = tree_data_type_const;
        using reference = const value_type &;
        using pointer = const value_type *;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_const_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_iterator;
        friend class btree_const_reverse_iterator;

        btree_const_iterator(const btree_iterator &it) noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self &operator++();
        self operator++(int);

        self &operator--();
        self operator--(int);

        bool operator==(const self &other) const noexcept;
        bool operator!=(const self &other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_const_iterator(const std::stack<std::pair<const btree_node **, size_t>> &path = std::stack<std::pair<const btree_node **, size_t>>(), size_t index = 0);
    };

    class btree_reverse_iterator final
    {
        std::stack<std::pair<btree_node **, size_t>> _path;
        size_t _index;

    public:
        using value_type = tree_data_type_const;
        using reference = value_type &;
        using pointer = value_type *;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_reverse_iterator;

        friend class B_tree;
        friend class btree_iterator;
        friend class btree_const_iterator;
        friend class btree_const_reverse_iterator;

        btree_reverse_iterator(const btree_iterator &it) noexcept;
        operator btree_iterator() const noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self &operator++();
        self operator++(int);

        self &operator--();
        self operator--(int);

        bool operator==(const self &other) const noexcept;
        bool operator!=(const self &other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_reverse_iterator(const std::stack<std::pair<btree_node **, size_t>> &path = std::stack<std::pair<btree_node **, size_t>>(), size_t index = 0);
    };

    class btree_const_reverse_iterator final
    {
        std::stack<std::pair<btree_node **, size_t>> _path;
        size_t _index;

    public:
        using value_type = tree_data_type_const;
        using reference = const value_type &;
        using pointer = const value_type *;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_const_reverse_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_const_iterator;
        friend class btree_iterator;

        btree_const_reverse_iterator(const btree_reverse_iterator &it) noexcept;
        operator btree_const_iterator() const noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self &operator++();
        self operator++(int);

        self &operator--();
        self operator--(int);

        bool operator==(const self &other) const noexcept;
        bool operator!=(const self &other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_const_reverse_iterator(const std::stack<std::pair<const btree_node **, size_t>> &path = std::stack<std::pair<const btree_node **, size_t>>(), size_t index = 0);
    };

    friend class btree_iterator;
    friend class btree_const_iterator;
    friend class btree_reverse_iterator;
    friend class btree_const_reverse_iterator;

    tvalue &at(const tkey &);
    const tvalue &at(const tkey &) const;

    tvalue &operator[](const tkey &key);
    tvalue &operator[](tkey &&key);

    btree_iterator begin();
    btree_iterator end();

    btree_const_iterator begin() const;
    btree_const_iterator end() const;

    btree_const_iterator cbegin() const;
    btree_const_iterator cend() const;

    btree_reverse_iterator rbegin();
    btree_reverse_iterator rend();

    btree_const_reverse_iterator rbegin() const;
    btree_const_reverse_iterator rend() const;

    btree_const_reverse_iterator crbegin() const;
    btree_const_reverse_iterator crend() const;

    size_t size() const noexcept;
    bool empty() const noexcept;

    btree_iterator find(const tkey &key);
    btree_const_iterator find(const tkey &key) const;

    btree_iterator lower_bound(const tkey &key);
    btree_const_iterator lower_bound(const tkey &key) const;

    btree_iterator upper_bound(const tkey &key);
    btree_const_iterator upper_bound(const tkey &key) const;

    bool contains(const tkey &key) const;

    void clear() noexcept;

    std::pair<btree_iterator, bool> insert(const tree_data_type &data);
    std::pair<btree_iterator, bool> insert(tree_data_type &&data);

    template <typename... Args>
    std::pair<btree_iterator, bool> emplace(Args &&...args);

    btree_iterator insert_or_assign(const tree_data_type &data);
    btree_iterator insert_or_assign(tree_data_type &&data);

    template <typename... Args>
    btree_iterator emplace_or_assign(Args &&...args);

    btree_iterator erase(btree_iterator pos);
    btree_iterator erase(btree_const_iterator pos);

    btree_iterator erase(btree_iterator beg, btree_iterator en);
    btree_iterator erase(btree_const_iterator beg, btree_const_iterator en);

    btree_iterator erase(const tkey &key);
};

template <std::input_iterator iterator, compator<typename std::iterator_traits<iterator>::value_type::first_type> compare = std::less<typename std::iterator_traits<iterator>::value_type::first_type>,
          std::size_t t = 5, typename U>
B_tree(iterator begin, iterator end, const compare &cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
       logger *logger = nullptr) -> B_tree<typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare, t>;

template <typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5, typename U>
B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare &cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
       logger *logger = nullptr) -> B_tree<tkey, tvalue, compare, t>;

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::compare_pairs(const B_tree::tree_data_type &lhs,
                                                     const B_tree::tree_data_type &rhs) const
{
    return compare_keys(lhs.first, rhs.first);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::compare_keys(const tkey &lhs, const tkey &rhs) const
{
    return compare::operator()(lhs, rhs);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_node::btree_node() noexcept
{
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
logger *B_tree<tkey, tvalue, compare, t>::get_logger() const noexcept
{
    return this->_logger;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
pp_allocator<typename B_tree<tkey, tvalue, compare, t>::value_type> B_tree<tkey, tvalue, compare, t>::get_allocator() const noexcept
{
    return this->_allocator;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(
    const compare &cmp,
    pp_allocator<value_type> alloc,
    logger *logger)
    : compare(cmp),
      _allocator(alloc),
      _logger(logger),
      _root(nullptr),
      _size(0)
{
    if (this->_logger)
    {
        this->_logger->trace("B_tree main constructor (cmp, alloc, logger) called.");
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(
    pp_allocator<value_type> alloc,
    const compare &comp,
    logger *logger_arg)
    : compare(comp),
      _allocator(alloc),
      _logger(logger_arg),
      _root(nullptr),
      _size(0)
{
    if (this->_logger)
    {
        this->_logger->trace("B_tree main constructor (alloc, cmp, logger) called.");
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template <input_iterator_for_pair<tkey, tvalue> iterator>
B_tree<tkey, tvalue, compare, t>::B_tree(
    iterator begin,
    iterator end,
    const compare &cmp,
    pp_allocator<value_type> alloc,
    logger *logger)
    : B_tree(cmp, alloc, logger)
{
    if (this->_logger)
    {
        this->_logger->trace("B_tree range constructor called.");
    }
    for (iterator current_it = begin; current_it != end; ++current_it)
    {
        this->insert(*current_it);
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(
    std::initializer_list<std::pair<tkey, tvalue>> data,
    const compare &cmp,
    pp_allocator<value_type> alloc,
    logger *logger)
    : B_tree(cmp, alloc, logger)
{
    if (this->_logger)
    {
        this->_logger->trace("B_tree initializer_list constructor called.");
    }

    for (const auto &item : data)
    {
        this->insert(item);
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::~B_tree() noexcept
{
    if (_logger)
        _logger->trace("B_tree destructor called.");
    clear();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(const B_tree &other)
    : compare(static_cast<const compare &>(other)),
      _allocator(other._allocator.select_on_container_copy_construction()),
      _logger(other._logger),
      _root(nullptr),
      _size(other._size)
{
    if (_logger)
        _logger->trace("B_tree copy constructor (iterative with std::map) called.");

    if (!other._root)
    {
        return;
    }

    pp_allocator<btree_node> node_alloc(this->_allocator);

    std::map<btree_node, btree_node> original_to_copy_map;

    std::stack<btree_node> nodes_to_process_original;
    nodes_to_process_original.push(other._root);

    while (!nodes_to_process_original.empty())
    {
        btree_node orig_node = nodes_to_process_original.top();
        nodes_to_process_original.pop();

        if (!orig_node)
            continue;

        btree_node new_node = node_alloc.new_object();
        new_node->_pointers.empty() = orig_node->_pointers.empty();
        new_node->_keys.size() = orig_node->_keys.size();
        new_node->_keys = orig_node->_keys;

        original_to_copy_map[orig_node] = new_node;

        if (!orig_node->_pointers.empty())
        {

            size_t num_children = orig_node->_keys.size() + 1;
            for (size_t i = 0; i < num_children; ++i)
            {
                size_t child_idx_in_orig = num_children - 1 - i;
                if (child_idx_in_orig < orig_node->_pointers.size() &&
                    orig_node->_pointers[child_idx_in_orig] != nullptr)
                {
                    nodes_to_process_original.push(orig_node->_pointers[child_idx_in_orig]);
                }
            }
        }
    }

    if (other._root)
    {
        this->_root = original_to_copy_map.at(other._root);

        for (const auto &pair_orig_copy : original_to_copy_map)
        {
            btree_node orig_node = pair_orig_copy.first;
            btree_node copied_node = pair_orig_copy.second;

            if (!orig_node->_pointers.empty())
            {
                size_t num_children = orig_node->_keys.size() + 1;
                if (num_children > 0 && num_children <= copied_node->_pointers.capacity())
                {
                    copied_node->_pointers.resize(num_children);
                }
                else if (num_children > copied_node->_pointers.capacity())
                {
                    if (_logger)
                        _logger->error("Iterative copy (map, 2nd pass): children capacity error.");
                    continue;
                }

                for (size_t i = 0; i < num_children; ++i)
                {
                    if (i < orig_node->_pointers.size() && orig_node->_pointers[i] != nullptr)
                    {

                        auto it_child_copy = original_to_copy_map.find(orig_node->_pointers[i]);
                        if (it_child_copy != original_to_copy_map.end())
                        {
                            copied_node->_pointers[i] = it_child_copy->second;
                        }
                        else
                        {

                            if (_logger)
                                _logger->error("Iterative copy (map, 2nd pass): child copy not found.");
                            copied_node->_pointers[i] = nullptr;
                        }
                    }
                    else
                    {
                        copied_node->_pointers[i] = nullptr;
                    }
                }
            }
        }
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t> &B_tree<tkey, tvalue, compare, t>::operator=(const B_tree &other)
{
    if (this->_logger)
    {
        this->_logger->trace("B_tree copy assignment operator called.");
    }

    if (this == &other)
    {
        return *this;
    }

    this->clear();

    static_cast<compare &>(*this) = static_cast<const compare &>(other);

    this->_allocator = other._allocator;
    this->_logger = other._logger;
    this->_size = other._size;

    if (!other._root)
    {
        this->_root = nullptr;
    }
    else
    {
        pp_allocator<btree_node> node_alloc(this->_allocator);
        std::map<btree_node, btree_node> original_to_copy_map;
        std::stack<btree_node> nodes_to_process_original;

        nodes_to_process_original.push(other._root);

        while (!nodes_to_process_original.empty())
        {
            btree_node orig_node = nodes_to_process_original.top();
            nodes_to_process_original.pop();
            if (!orig_node)
                continue;
            btree_node new_node = node_alloc.new_object();
            new_node->_pointers.empty() = orig_node->_pointers.empty();
            new_node->_keys.size() = orig_node->_keys.size();
            new_node->_keys = orig_node->_keys;
            original_to_copy_map[orig_node] = new_node;
            if (!orig_node->_pointers.empty())
            {
                size_t num_children = orig_node->_keys.size() + 1;
                for (size_t i = 0; i < num_children; ++i)
                {
                    size_t child_idx_in_orig = num_children - 1 - i;
                    if (child_idx_in_orig < orig_node->_pointers.size() && orig_node->_pointers[child_idx_in_orig] != nullptr)
                    {
                        nodes_to_process_original.push(orig_node->_pointers[child_idx_in_orig]);
                    }
                }
            }
        }

        auto root_it = original_to_copy_map.find(other._root);
        if (root_it != original_to_copy_map.end())
        {
            this->_root = root_it->second;
        }
        else
        {
            if (this->_logger)
                this->_logger->error("Root copy not found in map during copy assignment.");
            this->clear();
            return *this;
        }
        for (const auto &pair_orig_to_copy : original_to_copy_map)
        {
            btree_node orig_node = pair_orig_to_copy.first;
            btree_node copied_node = pair_orig_to_copy.second;
            if (!orig_node->_pointers.empty())
            {
                size_t num_children = orig_node->_keys.size() + 1;
                if (num_children > 0 && num_children <= copied_node->_pointers.capacity())
                {
                    copied_node->_pointers.resize(num_children);
                }
                else if (num_children > copied_node->_pointers.capacity())
                {
                    if (this->_logger)
                        this->_logger->error("Iterative copy assignment (map, 2nd pass): children capacity error.");
                    continue;
                }
                for (size_t i = 0; i < num_children; ++i)
                {
                    if (i < orig_node->_pointers.size() && orig_node->_pointers[i] != nullptr)
                    {
                        auto it_child_copy = original_to_copy_map.find(orig_node->_pointers[i]);
                        if (it_child_copy != original_to_copy_map.end())
                        {
                            copied_node->_pointers[i] = it_child_copy->second;
                        }
                        else
                        {
                            if (this->_logger)
                                this->_logger->error("Iterative copy assignment (map, 2nd pass): child copy not found.");
                            copied_node->_pointers[i] = nullptr;
                        }
                    }
                    else
                    {
                        copied_node->_pointers[i] = nullptr;
                    }
                }
            }
        }
    }
    return *this;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(B_tree &&other) noexcept
    : compare(std::move(static_cast<compare &>(other))),
      _allocator(std::move(other._allocator)),
      _logger(other._logger),
      _root(other._root),
      _size(other._size)
{
    if (this->_logger)
    {
        this->_logger->trace("B_tree move constructor called.");
    }

    other._root = nullptr;
    other._size = 0;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t> &B_tree<tkey, tvalue, compare, t>::operator=(B_tree &&other) noexcept
{
    if (other._logger)
    {
        other._logger->trace("B_tree move assignment operator called.");
    }
    else if (this->_logger)
    {
        this->_logger->trace("B_tree move assignment operator called (other had no logger).");
    }
    if (this == &other)
    {
        return *this;
    }
    this->clear();
    static_cast<compare &>(*this) = std::move(static_cast<compare &>(other));
    this->_allocator = std::move(other._allocator);
    this->_logger = other._logger;
    this->_root = other._root;
    this->_size = other._size;
    other._root = nullptr;
    other._size = 0;
    other._logger = nullptr;
    return *this;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_iterator::btree_iterator(
    const std::stack<std::pair<btree_node **, size_t>> &path, size_t index)
    : _path(path), _index(index)
{
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator*() const noexcept
{
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *(_path.top().first);
    return reinterpret_cast<reference>(current_node->_keys[_index]);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator->() const noexcept
{
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *(_path.top().first);
    return reinterpret_cast<pointer>(&(current_node->_keys[_index]));
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator &
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++()
{
    if (_path.empty())
    {
        return *this;
    }

    auto current_path_top = _path.top();
    typename B_tree<tkey, tvalue, compare, t>::btree_node **current_node_ptr_ptr = current_path_top.first;
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *current_node_ptr_ptr;

    if (!current_node->_pointers.empty())
    {

        size_t right_child_idx = _index + 1;
        // если есть правый ребенок, идем туда
        if (right_child_idx < current_node->_pointers.size() && current_node->_pointers[right_child_idx] != nullptr)
        {

            typename B_tree<tkey, tvalue, compare, t>::btree_node **next_node_ptr_ptr = &(current_node->_pointers[right_child_idx]);
            typename B_tree<tkey, tvalue, compare, t>::btree_node *next_node_obj = *next_node_ptr_ptr;
            _path.push({next_node_ptr_ptr, right_child_idx});

            while (next_node_obj != nullptr && !next_node_obj->_pointers.empty())
            {

                if (!next_node_obj->_pointers.empty() && next_node_obj->_pointers[0] != nullptr)
                {
                    typename B_tree<tkey, tvalue, compare, t>::btree_node **leftmost_child_ptr_ptr = &(next_node_obj->_pointers[0]);
                    _path.push({leftmost_child_ptr_ptr, 0});
                    next_node_obj = *leftmost_child_ptr_ptr;
                }
                else
                {
                    break;
                }
            }
            _index = 0;
            return *this;
        }
    }
    _index++; // нет
    if (_index < current_node->_keys.size())
    {
        return *this;
    }

    while (!_path.empty())
    { // увы наверх
        size_t parent_child_idx = _path.top().second;
        _path.pop();

        if (_path.empty())
        {
            _index = 0;
            return *this;
        }
        typename B_tree<tkey, tvalue, compare, t>::btree_node *parent_node = *(_path.top().first);

        if (parent_child_idx < parent_node->_keys.size())
        {
            _index = parent_child_idx;
            return *this;
        }
    }
    _index = 0;
    return *this;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++(int)
{
    self temp = *this;
    ++(*this);
    return temp;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator &
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--()
{
    if (_path.empty())
    {
        throw std::out_of_range("Cannot decrement end iterator");
    }

    auto current_path_top = _path.top();
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *(current_path_top.first);

    if (!current_node->_pointers.empty())
    {
        size_t left_child_idx = _index;

        if (left_child_idx < current_node->_pointers.size() && current_node->_pointers[left_child_idx] != nullptr)
        {
            typename B_tree<tkey, tvalue, compare, t>::btree_node **next_node_ptr_ptr = &(current_node->_pointers[left_child_idx]);
            typename B_tree<tkey, tvalue, compare, t>::btree_node *next_node_obj = *next_node_ptr_ptr;
            _path.push({next_node_ptr_ptr, left_child_idx});

            while (next_node_obj != nullptr && !next_node_obj->_pointers.empty())
            {

                size_t rightmost_child_idx = next_node_obj->_keys.size();
                if (rightmost_child_idx < next_node_obj->_pointers.size() && next_node_obj->_pointers[rightmost_child_idx] != nullptr)
                {
                    typename B_tree<tkey, tvalue, compare, t>::btree_node **rightmost_child_ptr_ptr = &(next_node_obj->_pointers[rightmost_child_idx]);
                    _path.push({rightmost_child_ptr_ptr, rightmost_child_idx});
                    next_node_obj = *rightmost_child_ptr_ptr;
                }
                else
                {
                    break;
                }
            }

            _index = next_node_obj->_keys.size() - 1;
            if (next_node_obj->_keys.empty())
            {
                _index = static_cast<size_t>(-1);
                throw std::runtime_error("Decrement led to empty node");
            }
            return *this;
        }
    }

    if (_index > 0)
    {
        _index--;
        return *this;
    }
    while (!_path.empty())
    {
        size_t parent_child_idx = _path.top().second;
        _path.pop();

        if (_path.empty())
        {

            _index = static_cast<size_t>(-1);

            return *this;
        }
        typename B_tree<tkey, tvalue, compare, t>::btree_node *parent_node = *(_path.top().first);

        if (parent_child_idx > 0)
        {
            _index = parent_child_idx - 1;
            return *this;
        }
    }
    _index = static_cast<size_t>(-1);
    return *this;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--(int)
{
    self temp = *this;
    --(*this);
    return temp;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator==(const self &other) const noexcept
{
    if (_path.empty() && other._path.empty())
    {
        return true;
    }

    if (_path.empty() || other._path.empty())
    {
        return false;
    }

    return (_path.top().first == other._path.top().first) && (_index == other._index);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator!=(const self &other) const noexcept
{
    return !(*this == other);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::depth() const noexcept
{
    if (_path.empty())
    {
        return 0;
    }
    return _path.size() - 1;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::current_node_keys_count() const noexcept
{
    if (_path.empty())
    {
        return 0;
    }
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *(_path.top().first);
    return current_node->_keys.size();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::is_terminate_node() const noexcept
{
    if (_path.empty())
    {
        return false;
    }
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *(_path.top().first);
    return current_node->_pointers.empty();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::index() const noexcept
{
    return _index;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(
    const std::stack<std::pair<const btree_node **, size_t>> &path, size_t index)
    : _path(path), _index(index)
{
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(
    const btree_iterator &it) noexcept
    : _path(it._path), _index(it._index)
{
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator*() const noexcept
{
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *(_path.top().first);
    return reinterpret_cast<reference>(current_node->_keys[_index]);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator->() const noexcept
{
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *(_path.top().first);
    tree_data_type *address_of_element = &(current_node->_keys[_index]);
    return reinterpret_cast<pointer>(address_of_element);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator &
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++()
{
    btree_iterator temp_non_const_it(_path, _index);
    ++temp_non_const_it;
    this->_path = temp_non_const_it._path;
    this->_index = temp_non_const_it._index;

    return *this;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++(int)
{
    self temp = *this;
    ++(*this);
    return temp;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator &
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--()
{
    btree_iterator temp_non_const_it(_path, _index);
    --temp_non_const_it;
    this->_path = temp_non_const_it._path;
    this->_index = temp_non_const_it._index;
    return *this;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--(int)
{
    self temp = *this;
    --(*this);
    return temp;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator==(const self &other) const noexcept
{
    if (_path.empty() && other._path.empty())
        return true;
    if (_path.empty() || other._path.empty())
        return false;
    return (_path.top().first == other._path.top().first) && (_index == other._index);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator!=(const self &other) const noexcept
{
    return !(*this == other);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::depth() const noexcept
{
    if (_path.empty())
        return 0;
    return _path.size() - 1;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::current_node_keys_count() const noexcept
{
    if (_path.empty())
        return 0;
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *(_path.top().first);
    return current_node->_keys.size();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::is_terminate_node() const noexcept
{
    if (_path.empty())
        return false;
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *(_path.top().first);
    return current_node->_pointers.empty();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::index() const noexcept
{
    return _index;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(
    const std::stack<std::pair<btree_node **, size_t>> &path, size_t index)
    : _path(path), _index(index)
{
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(
    const btree_iterator &it) noexcept
    : _path(it._path), _index(it._index)
{
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator B_tree<tkey, tvalue, compare, t>::btree_iterator() const noexcept
{
    return btree_iterator(_path, _index);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator*() const noexcept
{
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *(_path.top().first);
    return reinterpret_cast<reference>(current_node->_keys[_index]);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator->() const noexcept
{
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *(_path.top().first);
    return reinterpret_cast<pointer>(&(current_node->_keys[_index]));
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator &
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++()
{
    btree_iterator temp_base_iterator(_path, _index);
    --temp_base_iterator;
    this->_path = temp_base_iterator._path;
    this->_index = temp_base_iterator._index;
    return *this;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++(int)
{
    self temp = *this;
    ++(*this);
    return temp;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator &
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--()
{
    btree_iterator temp_base_iterator(_path, _index);
    ++temp_base_iterator;
    this->_path = temp_base_iterator._path;
    this->_index = temp_base_iterator._index;
    return *this;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--(int)
{
    self temp = *this;
    --(*this);
    return temp;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator==(const self &other) const noexcept
{
    if (_path.empty() && other._path.empty())
        return true;
    if (_path.empty() || other._path.empty())
        return false;
    return (_path.top().first == other._path.top().first) && (_index == other._index);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator!=(const self &other) const noexcept
{
    return !(*this == other);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::depth() const noexcept
{
    if (_path.empty())
        return 0;
    return _path.size() - 1;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::current_node_keys_count() const noexcept
{
    if (_path.empty())
        return 0;
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *(_path.top().first);
    return current_node->_keys.size();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::is_terminate_node() const noexcept
{
    if (_path.empty())
        return false;
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *(_path.top().first);
    return current_node->_pointers.empty();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::index() const noexcept
{
    return _index;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(
    const std::stack<std::pair<const btree_node **, size_t>> &path, size_t index)
    : _path(path), _index(index)
{
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(
    const btree_reverse_iterator &it) noexcept
    : _path(it._path), _index(it._index)
{
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator B_tree<tkey, tvalue, compare, t>::btree_const_iterator() const noexcept
{
    return btree_const_iterator(_path, _index);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator*() const noexcept
{
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *(_path.top().first);
    return reinterpret_cast<reference>(current_node->_keys[_index]);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator->() const noexcept
{
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *(_path.top().first);
    return reinterpret_cast<pointer>(&(current_node->_keys[_index]));
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator &
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++()
{
    btree_const_iterator temp_base_iterator(_path, _index);
    --temp_base_iterator;

    this->_path = temp_base_iterator._path;
    this->_index = temp_base_iterator._index;
    return *this;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++(int)
{
    self temp = *this;
    ++(*this);
    return temp;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator &
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--()
{
    btree_const_iterator temp_base_iterator(_path, _index);
    ++temp_base_iterator;
    this->_path = temp_base_iterator._path;
    this->_index = temp_base_iterator._index;
    return *this;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--(int)
{
    self temp = *this;
    --(*this);
    return temp;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator==(const self &other) const noexcept
{
    bool this_is_end = this->_path.empty();
    bool other_is_end = other._path.empty();

    if (this_is_end && other_is_end)
    {
        return true;
    }
    if (this_is_end || other_is_end)
    {
        return false;
    }
    return (*this->_path.top().first == *other._path.top().first) &&
           (this->_index == other._index);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator!=(const self &other) const noexcept
{
    return !(*this == other);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::depth() const noexcept
{
    if (_path.empty())
        return 0;
    return _path.size() - 1;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::current_node_keys_count() const noexcept
{
    if (_path.empty())
        return 0;
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *(_path.top().first);
    return current_node->_keys.size();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::is_terminate_node() const noexcept
{
    if (_path.empty())
        return false;
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node = *(_path.top().first);
    return current_node->_pointers.empty();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::index() const noexcept
{
    return _index;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue &B_tree<tkey, tvalue, compare, t>::at(const tkey &key)
{
    if (this->_logger)
        this->_logger->trace("B_tree non-const at() called.");
    btree_iterator it = this->find(key);
    if (it == this->end())
    {
        throw std::out_of_range("Key not found in B_tree::at()");
    }
    return it->second;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
const tvalue &B_tree<tkey, tvalue, compare, t>::at(const tkey &key) const
{
    if (this->_logger)
        this->_logger->trace("B_tree const at() called.");
    btree_const_iterator it = this->find(key);
    if (it == this->cend())
    {
        throw std::out_of_range("Key not found in B_tree::at() const");
    }
    return it->second;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue &B_tree<tkey, tvalue, compare, t>::operator[](const tkey &key)
{
    if (this->_logger)
        this->_logger->trace("B_tree operator[](const key&) called.");
    btree_iterator it = this->lower_bound(key);
    if (it != this->end() && !this->compare_keys(key, it->first) && !this->compare_keys(it->first, key))
    {
        return it->second;
    }
    else
    {
        return this->emplace(key, tvalue{}).first->second;
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue &B_tree<tkey, tvalue, compare, t>::operator[](tkey &&key)
{
    if (this->_logger)
        this->_logger->trace("B_tree operator[](key&&) called.");
    btree_iterator it = this->lower_bound(key);

    if (it != this->end() && !this->compare_keys(key, it->first) && !this->compare_keys(it->first, key))
    {
        return it->second;
    }

    else
    {
        return this->emplace(std::move(key), tvalue{}).first->second;
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::begin()
{
    if (this->_logger)
        this->_logger->trace("B_tree begin() called.");
    if (!this->_root)
    {
        return this->end();
    }
    std::stack<std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_node **, size_t>> path_stack;

    typename B_tree<tkey, tvalue, compare, t>::btree_node **current_node_ptr_ptr = &this->_root;
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node_obj = this->_root;
    size_t child_idx_from_parent = static_cast<size_t>(-1);

    while (current_node_obj != nullptr && !current_node_obj->_pointers.empty())
    {
        path_stack.push({current_node_ptr_ptr, child_idx_from_parent});
        if (current_node_obj->_pointers.empty() || current_node_obj->_pointers[0] == nullptr)
        {
            break;
        }

        child_idx_from_parent = 0;
        current_node_ptr_ptr = &(current_node_obj->_pointers[0]);
        current_node_obj = *current_node_ptr_ptr;
    }

    if (current_node_obj)
    {
        path_stack.push({current_node_ptr_ptr, child_idx_from_parent});
        if (!current_node_obj->_keys.empty())
        {
            return btree_iterator(path_stack, 0);
        }
        else
        {
            if (this->_logger)
                this->_logger->warning("begin() found an empty leaf node (unexpected).");
            return this->end();
        }
    }
    return this->end();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::end()
{
    return btree_iterator();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::begin() const
{
    if (this->_logger)
        this->_logger->trace("B_tree const begin() calling non-const version.");
    return const_cast<B_tree *>(this)->begin();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::end() const
{
    return const_cast<B_tree *>(this)->end();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cbegin() const
{
    return this->begin();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cend() const
{
    return this->end();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin()
{
    if (this->_logger)
        this->_logger->trace("B_tree rbegin() called.");
    if (!this->_root)
    {
        return this->rend();
    }

    std::stack<std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_node **, size_t>> path_stack;
    typename B_tree<tkey, tvalue, compare, t>::btree_node **current_node_ptr_ptr = &this->_root;
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node_obj = this->_root;
    size_t child_idx_from_parent = static_cast<size_t>(-1);

    while (current_node_obj != nullptr && !current_node_obj->_pointers.empty())
    {
        path_stack.push({current_node_ptr_ptr, child_idx_from_parent});
        size_t rightmost_child_idx = current_node_obj->_keys.size();
        if (rightmost_child_idx >= current_node_obj->_pointers.size() || current_node_obj->_pointers[rightmost_child_idx] == nullptr)
        {
            break;
        }
        child_idx_from_parent = rightmost_child_idx;
        current_node_ptr_ptr = &(current_node_obj->_pointers[rightmost_child_idx]);
        current_node_obj = *current_node_ptr_ptr;
    }

    if (current_node_obj)
    {
        path_stack.push({current_node_ptr_ptr, child_idx_from_parent});
        if (!current_node_obj->_keys.empty())
        {
            return btree_reverse_iterator(path_stack, current_node_obj->_keys.size() - 1);
        }
        else
        {
            if (this->_logger)
                this->_logger->warning("rbegin() found an empty leaf node (unexpected).");
            return this->rend();
        }
    }
    return this->rend();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend()
{
    return btree_reverse_iterator();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin() const
{
    if (this->_logger)
        this->_logger->trace("B_tree const rbegin() calling non-const version.");

    return const_cast<B_tree *>(this)->rbegin();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend() const
{
    return const_cast<B_tree *>(this)->rend();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crbegin() const
{
    return this->rbegin();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crend() const
{
    return this->rend();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::size() const noexcept
{
    return this->_size;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::empty() const noexcept
{
    return this->_size == 0;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::find(const tkey &key)
{
    if (this->_logger)
        this->_logger->trace("B_tree const find called.");
    if (!this->_root)
    {
        return this->end();
    }
    std::stack<std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_node **, size_t>> path_stack;

    typename B_tree<tkey, tvalue, compare, t>::btree_node **current_node_ptr_ptr =
        const_cast<typename B_tree<tkey, tvalue, compare, t>::btree_node **>(&this->_root);
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node_obj = this->_root;
    size_t child_idx_from_parent = static_cast<size_t>(-1);
    while (current_node_obj != nullptr)
    {
        path_stack.push({current_node_ptr_ptr, child_idx_from_parent});
        auto lower_it = std::lower_bound(
            current_node_obj->_keys.begin(),
            current_node_obj->_keys.begin() + current_node_obj->_keys.size(),
            key,
            [this](const tree_data_type &pair_in_node, const tkey &key_to_find)
            {
                return this->compare_keys(pair_in_node.first, key_to_find);
            });
        size_t key_idx_in_node = std::distance(current_node_obj->_keys.begin(), lower_it);
        if (key_idx_in_node < current_node_obj->_keys.size() &&
            !this->compare_keys(key, current_node_obj->_keys[key_idx_in_node].first))
        {
            return btree_iterator(path_stack, key_idx_in_node);
        }
        if (current_node_obj->_pointers.empty())
        {
            return this->end();
        }

        else
        {
            if (key_idx_in_node >= current_node_obj->_pointers.size() || current_node_obj->_pointers[key_idx_in_node] == nullptr)
            {
                if (this->_logger)
                    this->_logger->error("find: Invalid descent path encountered.");
                return this->end();
            }
            child_idx_from_parent = key_idx_in_node;
            current_node_ptr_ptr = const_cast<typename B_tree<tkey, tvalue, compare, t>::btree_node **>(
                &(current_node_obj->_pointers[child_idx_from_parent]));
            current_node_obj = current_node_obj->_pointers[child_idx_from_parent];
        }
    }
    return this->end();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::find(const tkey &key) const
{
    if (this->_logger)
        this->_logger->trace("B_tree non-const find called.");
    if (!this->_root)
        return this->end();
    std::stack<std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_node **, size_t>> path_stack;
    typename B_tree<tkey, tvalue, compare, t>::btree_node **current_node_ptr_ptr = &this->_root;
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node_obj = this->_root;
    size_t child_idx_from_parent = static_cast<size_t>(-1);

    while (current_node_obj != nullptr)
    {
        path_stack.push({current_node_ptr_ptr, child_idx_from_parent});
        auto lower_it = std::lower_bound(
            current_node_obj->_keys.begin(), current_node_obj->_keys.begin() + current_node_obj->_keys.size(), key,
            [this](const tree_data_type &p, const tkey &k)
            { return this->compare_keys(p.first, k); });
        size_t key_idx_in_node = std::distance(current_node_obj->_keys.begin(), lower_it);

        if (key_idx_in_node < current_node_obj->_keys.size() && !this->compare_keys(key, current_node_obj->_keys[key_idx_in_node].first))
        {
            return btree_const_iterator(path_stack, key_idx_in_node);
        }
        if (current_node_obj->_pointers.empty())
        {
            return this->end();
        }
        else
        {
            if (key_idx_in_node >= current_node_obj->_pointers.size() || current_node_obj->_pointers[key_idx_in_node] == nullptr)
            {
                return this->cend();
            }
            child_idx_from_parent = key_idx_in_node;
            current_node_ptr_ptr = &(current_node_obj->_pointers[child_idx_from_parent]);
            current_node_obj = current_node_obj->_pointers[child_idx_from_parent];
        }
    }
    return this->cend();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey &key)
{
    if (this->_logger)
        this->_logger->trace("B_tree non-const lower_bound called.");
    if (!this->_root)
    {
        return this->end();
    }

    std::stack<std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_node **, size_t>> path_stack;
    typename B_tree<tkey, tvalue, compare, t>::btree_node **current_node_ptr_ptr = &this->_root;
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node_obj = this->_root;
    size_t child_idx_from_parent = static_cast<size_t>(-1);

    std::stack<std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_node **, size_t>> best_path_stack;
    size_t best_key_idx = static_cast<size_t>(-1);

    while (current_node_obj != nullptr)
    {
        path_stack.push({current_node_ptr_ptr, child_idx_from_parent});

        auto lower_it = std::lower_bound(
            current_node_obj->_keys.begin(),
            current_node_obj->_keys.begin() + current_node_obj->_keys.size(),
            key,
            [this](const tree_data_type &p, const tkey &k)
            { return this->compare_keys(p.first, k); });
        size_t key_idx_in_node = std::distance(current_node_obj->_keys.begin(), lower_it);
        if (key_idx_in_node < current_node_obj->_keys.size())
        {
            if (!this->compare_keys(current_node_obj->_keys[key_idx_in_node].first, key))
            {
                best_path_stack = path_stack;
                best_key_idx = key_idx_in_node;
            }
        }
        if (current_node_obj->_pointers.empty())
        {
            break;
        }
        else
        {
            if (key_idx_in_node >= current_node_obj->_pointers.size() || current_node_obj->_pointers[key_idx_in_node] == nullptr)
            {

                break;
            }
            child_idx_from_parent = key_idx_in_node;
            current_node_ptr_ptr = &(current_node_obj->_pointers[child_idx_from_parent]);
            current_node_obj = current_node_obj->_pointers[child_idx_from_parent];
        }
    }
    if (best_key_idx != static_cast<size_t>(-1))
    {
        return btree_iterator(best_path_stack, best_key_idx);
    }
    else
    {
        return this->end();
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey &key) const
{
    if (this->_logger)
        this->_logger->trace("B_tree const lower_bound called.");

    if (!this->_root)
        return this->cend();
    std::stack<std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_node **, size_t>> path_stack;
    typename B_tree<tkey, tvalue, compare, t>::btree_node **current_node_ptr_ptr = const_cast<typename B_tree<tkey, tvalue, compare, t>::btree_node **>(&this->_root);
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node_obj = this->_root;
    size_t child_idx_from_parent = static_cast<size_t>(-1);
    std::stack<std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_node **, size_t>> best_path_stack;
    size_t best_key_idx = static_cast<size_t>(-1);

    while (current_node_obj != nullptr)
    {
        path_stack.push({current_node_ptr_ptr, child_idx_from_parent});
        auto lower_it = std::lower_bound(
            current_node_obj->_keys.begin(), current_node_obj->_keys.begin() + current_node_obj->_keys.size(), key,
            [this](const tree_data_type &p, const tkey &k)
            { return this->compare_keys(p.first, k); });
        size_t key_idx_in_node = std::distance(current_node_obj->_keys.begin(), lower_it);

        if (key_idx_in_node < current_node_obj->_keys.size())
        {
            if (!this->compare_keys(current_node_obj->_keys[key_idx_in_node].first, key))
            {
                best_path_stack = path_stack;
                best_key_idx = key_idx_in_node;
            }
        }
        if (current_node_obj->_pointers.empty())
        {
            break;
        }
        else
        {
            if (key_idx_in_node >= current_node_obj->_pointers.size() || current_node_obj->_pointers[key_idx_in_node] == nullptr)
            {
                break;
            }
            child_idx_from_parent = key_idx_in_node;
            current_node_ptr_ptr = const_cast<typename B_tree<tkey, tvalue, compare, t>::btree_node **>(&(current_node_obj->_pointers[child_idx_from_parent]));
            current_node_obj = current_node_obj->_pointers[child_idx_from_parent];
        }
    }
    if (best_key_idx != static_cast<size_t>(-1))
    {
        return btree_const_iterator(best_path_stack, best_key_idx);
    }
    else
    {
        return this->cend();
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey &key)
{
    if (this->_logger)
        this->_logger->trace("B_tree non-const upper_bound called.");

    if (!this->_root)
        return this->end();
    std::stack<std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_node **, size_t>> path_stack;
    typename B_tree<tkey, tvalue, compare, t>::btree_node **current_node_ptr_ptr = &this->_root;
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node_obj = this->_root;
    size_t child_idx_from_parent = static_cast<size_t>(-1);
    std::stack<std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_node **, size_t>> best_path_stack;
    size_t best_key_idx = static_cast<size_t>(-1);

    while (current_node_obj != nullptr)
    {
        path_stack.push({current_node_ptr_ptr, child_idx_from_parent});
        auto upper_it = std::lower_bound( // определяем в какого потомка идти
            current_node_obj->_keys.begin(),
            current_node_obj->_keys.begin() + current_node_obj->_keys.size(),
            key,
            [this](const tkey &k, const tree_data_type &p)
            { return this->compare_keys(k, p.first); });

        size_t key_idx_in_node = std::distance(current_node_obj->_keys.begin(), upper_it);

        if (key_idx_in_node < current_node_obj->_keys.size())
        { // lets go
            best_path_stack = path_stack;
            best_key_idx = key_idx_in_node;
        }
        auto lower_it = std::lower_bound(
            current_node_obj->_keys.begin(),
            current_node_obj->_keys.begin() + current_node_obj->_keys.size(),
            key,
            [this](const tree_data_type &p, const tkey &k)
            { return this->compare_keys(p.first, k); });
        size_t descend_idx = std::distance(current_node_obj->_keys.begin(), lower_it);

        if (current_node_obj->_pointers.empty())
        {
            break;
        }
        else
        {
            if (descend_idx >= current_node_obj->_pointers.size() || current_node_obj->_pointers[descend_idx] == nullptr)
            {
                break;
            }
            child_idx_from_parent = descend_idx;
            current_node_ptr_ptr = &(current_node_obj->_pointers[child_idx_from_parent]);
            current_node_obj = current_node_obj->_pointers[child_idx_from_parent];
        }
    }
    if (best_key_idx != static_cast<size_t>(-1))
    {
        return btree_iterator(best_path_stack, best_key_idx);
    }
    else
    {
        return this->end();
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey &key) const
{
    if (this->_logger)
        this->_logger->trace("B_tree const upper_bound called.");

    if (!this->_root)
        return this->cend();
    std::stack<std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_node **, size_t>> path_stack;
    typename B_tree<tkey, tvalue, compare, t>::btree_node **current_node_ptr_ptr = const_cast<typename B_tree<tkey, tvalue, compare, t>::btree_node **>(&this->_root);
    typename B_tree<tkey, tvalue, compare, t>::btree_node *current_node_obj = this->_root;
    size_t child_idx_from_parent = static_cast<size_t>(-1);
    std::stack<std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_node **, size_t>> best_path_stack;
    size_t best_key_idx = static_cast<size_t>(-1);

    while (current_node_obj != nullptr)
    {
        path_stack.push({current_node_ptr_ptr, child_idx_from_parent});
        auto upper_it = std::lower_bound(
            current_node_obj->_keys.begin(), current_node_obj->_keys.begin() + current_node_obj->_keys.size(), key,
            [this](const tkey &k, const tree_data_type &p)
            { return this->compare_keys(k, p.first); });
        size_t key_idx_in_node = std::distance(current_node_obj->_keys.begin(), upper_it);
        if (key_idx_in_node < current_node_obj->_keys.size())
        {
            best_path_stack = path_stack;
            best_key_idx = key_idx_in_node;
        }
        auto lower_it = std::lower_bound(
            current_node_obj->_keys.begin(), current_node_obj->_keys.begin() + current_node_obj->_keys.size(), key,
            [this](const tree_data_type &p, const tkey &k)
            { return this->compare_keys(p.first, k); });
        size_t descend_idx = std::distance(current_node_obj->_keys.begin(), lower_it);
        if (current_node_obj->_pointers.empty())
        {
            break;
        }
        else
        {
            if (descend_idx >= current_node_obj->_pointers.size() || current_node_obj->_pointers[descend_idx] == nullptr)
            {
                break;
            }
            child_idx_from_parent = descend_idx;
            current_node_ptr_ptr = const_cast<typename B_tree<tkey, tvalue, compare, t>::btree_node **>(&(current_node_obj->_pointers[child_idx_from_parent]));
            current_node_obj = current_node_obj->_pointers[child_idx_from_parent];
        }
    }
    if (best_key_idx != static_cast<size_t>(-1))
    {
        return btree_const_iterator(best_path_stack, best_key_idx);
    }
    else
    {
        return this->cend();
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::contains(const tkey &key) const
{
    if (this->_logger)
        this->_logger->trace("B_tree contains called.");

    return this->find(key) != this->cend();
}
// куда
template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::find_key_index_in_node(

    typename B_tree<tkey, tvalue, compare, t>::btree_node *node,
    const tkey &k) const
{
    auto it = std::lower_bound(
        node->_keys.begin(),
        node->_keys.begin() + node->_keys.size(),
        k,
        [this](const tree_data_type &pair_in_node, const tkey &key_to_find)
        {
            return this->compare_keys(pair_in_node.first, key_to_find);
        });
    return std::distance(node->_keys.begin(), it);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::erase_recursive(

    typename B_tree<tkey, tvalue, compare, t>::btree_node *node,
    const tkey &k)
{
    static constexpr std::size_t t_param = t;
    static constexpr size_t min_keys = t - 1;

    size_t idx = find_key_index_in_node(node, k);

    // уже есть
    if (idx < node->_keys.size() && !this->compare_keys(k, node->_keys[idx].first) && !this->compare_keys(node->_keys[idx].first, k))
    {
        if (node->_pointers.empty())
        { // лист
            remove_from_leaf(node, idx);
        }
        else
        { // внутр узел
            remove_from_internal(node, idx);
        }
    }

    else if (!node->_pointers.empty())
    { // ниже go
        if (idx >= node->_pointers.size())
        {
            if (this->_logger)
                this->_logger->error("erase_recursive: Attempting to descend to non-existent child index.");
            return;
        }
        // если в потомке мин допустимое кол-во ключей
        if (node->_pointers[idx]->_keys.size() == min_keys)
        {
            fill_child(node, idx);

            idx = find_key_index_in_node(node, k);
        }

        if (idx >= node->_pointers.size())
        {
            if (this->_logger)
                this->_logger->error("erase_recursive: Child index invalid after fill_child.");
            throw std::runtime_error("Child index invalid after fill_child.");
        }

        if (node->_pointers[idx] == nullptr)
        {
            if (this->_logger)
                this->_logger->error("erase_recursive: Attempting to call recursively on null child.");
            throw std::runtime_error("Attempting to call recursively on null child.");
        }
        erase_recursive(node->_pointers[idx], k);
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::remove_from_leaf(
    typename B_tree<tkey, tvalue, compare, t>::btree_node *node,
    size_t key_idx)
{
    if (key_idx < node->_keys.size())
    {
        node->_keys.erase(node->_keys.begin() + key_idx);
    }
    else
    {
        if (this->_logger)
            this->_logger->warning("remove_from_leaf called with invalid index.");
        throw std::logic_error("remove_from_leaf called with invalid index.");
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::remove_from_internal(
    typename B_tree<tkey, tvalue, compare, t>::btree_node *node,
    size_t key_idx)
{
    static constexpr std::size_t t_param = t;

    if (key_idx >= node->_keys.size() || (key_idx + 1) >= node->_pointers.size() || node->_pointers[key_idx] == nullptr || node->_pointers[key_idx + 1] == nullptr)
    {
        if (this->_logger)
            this->_logger->error("Invalid index or missing children in remove_from_internal.");
        throw std::logic_error("Invalid index or missing children in remove_from_internal.");
    }

    tkey k = node->_keys[key_idx].first;

    typename B_tree<tkey, tvalue, compare, t>::btree_node *pred_child = node->_pointers[key_idx];
    typename B_tree<tkey, tvalue, compare, t>::btree_node *succ_child = node->_pointers[key_idx + 1];

    if (pred_child->_keys.size() >= t)
    { // лпд содержит >= t ключей
        tree_data_type predecessor = get_predecessor(node, key_idx);
        tkey pred_key = predecessor.first;
        node->_keys[key_idx] = std::move(predecessor);
        erase_recursive(pred_child, pred_key);
    }
    else if (succ_child->_keys.size() >= t)
    { // ппд
        tree_data_type successor = get_successor(node, key_idx);
        tkey succ_key = successor.first;
        node->_keys[key_idx] = std::move(successor);
        erase_recursive(succ_child, succ_key);
    }
    else
    { // оба пдд содержат t-1 ключей
        merge_children(node, key_idx);
        if (key_idx < node->_pointers.size() && node->_pointers[key_idx] != nullptr)
        {
            erase_recursive(node->_pointers[key_idx], k);
        }
        else
        {
            if (this->_logger)
                this->_logger->error("Merged child missing in remove_from_internal.");
            throw std::runtime_error("Merged child missing in remove_from_internal.");
        }
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::tree_data_type
B_tree<tkey, tvalue, compare, t>::get_predecessor(
    typename B_tree<tkey, tvalue, compare, t>::btree_node *node,
    size_t key_idx)
{
    if (key_idx >= node->_pointers.size() || node->_pointers[key_idx] == nullptr)
    {
        throw std::logic_error("Invalid index or null child for get_predecessor.");
    }

    typename B_tree<tkey, tvalue, compare, t>::btree_node *current = node->_pointers[key_idx];
    while (!current->_pointers.empty())
    {
        if (current->_pointers.empty())
        {
            throw std::runtime_error("Accessing back() on empty pointers in get_predecessor");
        }
        if (current->_pointers.back() == nullptr)
        {
            throw std::runtime_error("Null pointer encountered during descent in get_predecessor.");
        }
        current = current->_pointers[current->_keys.size()];
    }
    if (current->_keys.empty())
    {
        throw std::runtime_error("Empty leaf node encountered while finding predecessor.");
    }
    return current->_keys.back();
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::tree_data_type
B_tree<tkey, tvalue, compare, t>::get_successor(
    typename B_tree<tkey, tvalue, compare, t>::btree_node *node,
    size_t key_idx)
{
    if ((key_idx + 1) >= node->_pointers.size() || node->_pointers[key_idx + 1] == nullptr)
    {
        throw std::logic_error("Invalid index or null child for get_successor.");
    }

    typename B_tree<tkey, tvalue, compare, t>::btree_node *current = node->_pointers[key_idx + 1];
    while (!current->_pointers.empty())
    {
        if (current->_pointers.empty())
        {
            throw std::runtime_error("Accessing front() on empty pointers in get_successor");
        }
        if (current->_pointers.front() == nullptr)
        {
            throw std::runtime_error("Null pointer encountered during descent in get_successor.");
        }
        current = current->_pointers.front();
    }
    if (current->_keys.empty())
    {
        throw std::runtime_error("Empty leaf node encountered while finding successor.");
    }
    return current->_keys.front();
}

// если меньше мин кол-ва ключей
template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::fill_child(
    typename B_tree<tkey, tvalue, compare, t>::btree_node *node,
    size_t child_idx)
{
    static constexpr std::size_t t_param = t;
    static constexpr size_t min_keys = t - 1;

    if (child_idx >= node->_pointers.size() || node->_pointers[child_idx] == nullptr)
    {
        throw std::logic_error("Invalid child index passed to fill_child.");
    }

    bool has_left_sibling = (child_idx > 0);
    bool left_sibling_valid = has_left_sibling && (node->_pointers[child_idx - 1] != nullptr);
    bool can_borrow_from_left = left_sibling_valid && (node->_pointers[child_idx - 1]->_keys.size() > min_keys);

    if (can_borrow_from_left)
    {
        borrow_from_prev(node, child_idx);
        return;
    }

    bool has_right_sibling_key = (child_idx < node->_keys.size());
    bool right_sibling_valid = has_right_sibling_key && ((child_idx + 1) < node->_pointers.size()) && (node->_pointers[child_idx + 1] != nullptr);
    bool can_borrow_from_right = right_sibling_valid && (node->_pointers[child_idx + 1]->_keys.size() > min_keys);

    if (can_borrow_from_right)
    { // иначе правый
        borrow_from_next(node, child_idx);
        return;
    }

    if (right_sibling_valid)
    { // увы сливаем ребенка с соседом
        merge_children(node, child_idx);
    }
    else if (left_sibling_valid)
    {
        merge_children(node, child_idx - 1);
    }
    else
    {
        throw std::logic_error("fill_child cannot balance node with no suitable siblings.");
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::borrow_from_prev(
    typename B_tree<tkey, tvalue, compare, t>::btree_node *node,
    size_t child_idx)
{
    if (child_idx == 0 || child_idx >= node->_pointers.size() || node->_pointers[child_idx] == nullptr ||
        (child_idx - 1) >= node->_pointers.size() || node->_pointers[child_idx - 1] == nullptr ||
        (child_idx - 1) >= node->_keys.size())
    {
        throw std::logic_error("Invalid indices/pointers for borrow_from_prev.");
    }

    typename B_tree<tkey, tvalue, compare, t>::btree_node *child = node->_pointers[child_idx];
    typename B_tree<tkey, tvalue, compare, t>::btree_node *sibling = node->_pointers[child_idx - 1];
    size_t parent_key_idx = child_idx - 1;

    if (sibling->_keys.empty())
        throw std::runtime_error("Borrowing key from empty sibling node.");
    bool sibling_is_leaf = sibling->_pointers.empty();
    if (!sibling_is_leaf && sibling->_pointers.empty())
        throw std::runtime_error("Borrowing pointer from childless internal sibling node.");

    child->_keys.insert(child->_keys.begin(), std::move(node->_keys[parent_key_idx]));
    node->_keys[parent_key_idx] = std::move(sibling->_keys.back());
    sibling->_keys.pop_back();

    if (!sibling_is_leaf)
    {
        if (child->_pointers.empty())
            throw std::runtime_error("Moving pointer into a leaf node (child).");
        child->_pointers.insert(child->_pointers.begin(), sibling->_pointers.back());
        sibling->_pointers.pop_back();
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::borrow_from_next(
    typename B_tree<tkey, tvalue, compare, t>::btree_node *node,
    size_t child_idx)
{
    if (child_idx >= node->_keys.size() || child_idx >= node->_pointers.size() || node->_pointers[child_idx] == nullptr ||
        (child_idx + 1) >= node->_pointers.size() || node->_pointers[child_idx + 1] == nullptr)
    {
        throw std::logic_error("Invalid indices/pointers for borrow_from_next.");
    }

    typename B_tree<tkey, tvalue, compare, t>::btree_node *child = node->_pointers[child_idx];
    typename B_tree<tkey, tvalue, compare, t>::btree_node *sibling = node->_pointers[child_idx + 1];
    size_t parent_key_idx = child_idx;

    if (sibling->_keys.empty())
        throw std::runtime_error("Borrowing key from empty sibling node.");
    bool sibling_is_leaf = sibling->_pointers.empty();
    if (!sibling_is_leaf && sibling->_pointers.empty())
        throw std::runtime_error("Borrowing pointer from childless internal sibling node.");

    child->_keys.push_back(std::move(node->_keys[parent_key_idx]));
    node->_keys[parent_key_idx] = std::move(sibling->_keys.front());
    sibling->_keys.erase(sibling->_keys.begin());

    if (!sibling_is_leaf)
    {
        if (child->_pointers.empty())
            throw std::runtime_error("Moving pointer into a leaf node (child).");
        child->_pointers.push_back(sibling->_pointers.front());
        sibling->_pointers.erase(sibling->_pointers.begin());
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::merge_children(
    typename B_tree<tkey, tvalue, compare, t>::btree_node *node,
    size_t child_idx)
{
    if (child_idx >= node->_keys.size() || child_idx >= node->_pointers.size() || node->_pointers[child_idx] == nullptr ||
        (child_idx + 1) >= node->_pointers.size() || node->_pointers[child_idx + 1] == nullptr)
    {
        throw std::logic_error("Invalid indices/pointers for merge_children.");
    }

    typename B_tree<tkey, tvalue, compare, t>::btree_node *left_child = node->_pointers[child_idx];
    typename B_tree<tkey, tvalue, compare, t>::btree_node *right_child = node->_pointers[child_idx + 1];
    size_t parent_key_idx = child_idx; // индекс ключа в родителе который делит этих детей двух
    bool left_is_leaf = left_child->_pointers.empty();
    if (left_is_leaf != right_child->_pointers.empty())
    {
        throw std::runtime_error("Merging leaf and internal node.");
    }

    left_child->_keys.push_back(std::move(node->_keys[parent_key_idx]));

    left_child->_keys.insert(left_child->_keys.end(),
                             std::make_move_iterator(right_child->_keys.begin()),
                             std::make_move_iterator(right_child->_keys.end()));
    right_child->_keys.clear();

    if (!left_is_leaf)
    {
        left_child->_pointers.insert(left_child->_pointers.end(),
                                     std::make_move_iterator(right_child->_pointers.begin()),
                                     std::make_move_iterator(right_child->_pointers.end()));
        right_child->_pointers.clear();
    }

    node->_keys.erase(node->_keys.begin() + parent_key_idx);
    node->_pointers.erase(node->_pointers.begin() + child_idx + 1);

    pp_allocator<btree_node> node_alloc = this->get_allocator();
    node_alloc.delete_object(right_child);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::clear() noexcept
{
    if (_logger)
        _logger->trace("B_tree clear() called.");
    if (!_root)
    {
        _size = 0;
        return;
    }

    std::stack<typename B_tree<tkey, tvalue, compare, t>::btree_node *> s1;
    std::stack<typename B_tree<tkey, tvalue, compare, t>::btree_node *> s2;

    s1.push(this->_root);

    while (!s1.empty())
    {

        typename B_tree<tkey, tvalue, compare, t>::btree_node *current = s1.top();
        s1.pop();

        if (current)
        {
            s2.push(current);
            if (!current->_pointers.empty())
            {
                for (size_t i = 0; i < current->_pointers.size(); ++i)
                {
                    if (current->_pointers[i] != nullptr)
                    {
                        s1.push(current->_pointers[i]);
                    }
                }
            }
        }
    }

    pp_allocator<btree_node> node_alloc = this->get_allocator();
    while (!s2.empty())
    {
        typename B_tree<tkey, tvalue, compare, t>::btree_node *node_to_free = s2.top();
        s2.pop();
        if (node_to_free)
        {
            node_alloc.delete_object(node_to_free);
        }
    }

    this->_root = nullptr;
    this->_size = 0;
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::insert(const tree_data_type &data)
{
    static constexpr std::size_t current_t = t;
    using Node = typename B_tree<tkey, tvalue, compare, t>::btree_node;
    using NodeStar = Node *;

    if (this->_logger)
    {
        this->_logger->trace("B_tree insert [HYBRID SPLIT] called.");
    }
    tkey key_copy = data.first;

    if (!this->_root)
    { // дерево пустое
        pp_allocator<Node> node_alloc(this->_allocator);
        this->_root = node_alloc.template new_object<Node>();
        this->_root->_keys.push_back(data);
        this->_size = 1;
        std::stack<std::pair<NodeStar *, size_t>> path_stack;
        path_stack.push({&this->_root, static_cast<size_t>(-1)});
        return {btree_iterator(path_stack, 0), true};
    }
    // если ключ есть уже
    btree_iterator existing_it = this->find(key_copy);
    if (existing_it != this->end())
    {
        return {existing_it, false};
    }

    if (this->_root->_keys.size() == (2 * current_t - 1))
    {
        pp_allocator<Node> node_alloc(this->_allocator);
        NodeStar old_root_node = this->_root;
        NodeStar new_empty_root = node_alloc.template new_object<Node>();

        new_empty_root->_pointers.push_back(old_root_node);
        this->_root = new_empty_root;

        NodeStar parent_for_split = this->_root;
        size_t child_idx_for_split = 0;
        NodeStar y_to_split = old_root_node;
        NodeStar z_new_sibling = node_alloc.template new_object<Node>(); // брат
        bool y_was_leaf_when_split = y_to_split->_pointers.empty();

        std::vector<tree_data_type> z_keys_buffer_root;
        std::vector<NodeStar> z_pointers_buffer_root;
        std::vector<tree_data_type> y_keys_buffer_root;
        std::vector<NodeStar> y_pointers_buffer_root;

        if (current_t >= y_to_split->_keys.size())
        {
            if (_logger)
                _logger->error("Split logic error (root V2): median index 'current_t' out of bounds.");
        }

        if (current_t >= 2)
            z_keys_buffer_root.reserve(current_t - 2);
        for (size_t j = 0; j < current_t - 2; ++j)
            z_keys_buffer_root.push_back(std::move(y_to_split->_keys[current_t + 1 + j]));
        if (!y_was_leaf_when_split && current_t >= 1)
        {
            z_pointers_buffer_root.reserve(current_t - 1);
            for (size_t j = 0; j < current_t - 1; ++j)
                if ((current_t + 1 + j) < y_to_split->_pointers.size())
                    z_pointers_buffer_root.push_back(y_to_split->_pointers[current_t + 1 + j]);
        }

        tree_data_type median_key_to_parent = std::move(y_to_split->_keys[current_t]);

        y_keys_buffer_root.reserve(current_t); // оставшиеся ключи в левый узел
        for (size_t j = 0; j < current_t; ++j)
            y_keys_buffer_root.push_back(std::move(y_to_split->_keys[j]));
        if (!y_was_leaf_when_split)
        {
            y_pointers_buffer_root.reserve(current_t + 1);
            for (size_t j = 0; j < current_t + 1; ++j)
                if (j < y_to_split->_pointers.size())
                    y_pointers_buffer_root.push_back(y_to_split->_pointers[j]);
        }

        z_new_sibling->_keys.assign(std::make_move_iterator(z_keys_buffer_root.begin()), std::make_move_iterator(z_keys_buffer_root.end()));
        if (!y_was_leaf_when_split)
            z_new_sibling->_pointers.assign(z_pointers_buffer_root.begin(), z_pointers_buffer_root.end());
        y_to_split->_keys.assign(std::make_move_iterator(y_keys_buffer_root.begin()), std::make_move_iterator(y_keys_buffer_root.end()));
        if (!y_was_leaf_when_split)
            y_to_split->_pointers.assign(y_pointers_buffer_root.begin(), y_pointers_buffer_root.end());

        parent_for_split->_pointers.insert(parent_for_split->_pointers.begin() + child_idx_for_split + 1, z_new_sibling);
        parent_for_split->_keys.insert(parent_for_split->_keys.begin() + child_idx_for_split, std::move(median_key_to_parent));
    }

    NodeStar *current_node_outer_ptr = &this->_root;
    pp_allocator<Node> node_alloc_loop(this->_allocator);

    while (true)
    {
        NodeStar current_node = *current_node_outer_ptr;

        if (current_node->_pointers.empty())
        {
            auto insert_pos_it = std::lower_bound(
                current_node->_keys.begin(), current_node->_keys.end(), key_copy,
                [this](const tree_data_type &p, const tkey &k)
                { return this->compare_keys(p.first, k); });
            current_node->_keys.insert(insert_pos_it, data);
            this->_size++;
            return {this->find(key_copy), true};
        }

        auto descend_it = std::lower_bound(
            current_node->_keys.begin(), current_node->_keys.end(), key_copy,
            [this](const tree_data_type &p, const tkey &k)
            { return this->compare_keys(p.first, k); });
        size_t next_child_idx = std::distance(current_node->_keys.begin(), descend_it);
        // индекс ребенка как расстояние
        if (next_child_idx >= current_node->_pointers.size() || current_node->_pointers[next_child_idx] == nullptr)
        {
            if (this->_logger)
                this->_logger->error("Invalid child index or null child pointer before checking fullness (loop HYBRID).");
        }
        NodeStar child_node_to_descend = current_node->_pointers[next_child_idx];
        // ребенок заполнен-сплит его
        if (child_node_to_descend->_keys.size() == maximum_keys_in_node)
        {
            NodeStar parent_for_split = current_node;
            size_t child_idx_for_split = next_child_idx;
            NodeStar y_to_split = child_node_to_descend;
            NodeStar z_new_sibling = node_alloc_loop.template new_object<Node>();
            bool y_was_leaf_when_split = y_to_split->_pointers.empty();

            std::vector<tree_data_type> z_keys_buffer_loop;
            std::vector<NodeStar> z_pointers_buffer_loop;
            std::vector<tree_data_type> y_final_keys_buffer_loop;
            std::vector<NodeStar> y_final_pointers_buffer_loop;

            z_keys_buffer_loop.reserve(current_t - 1);
            for (size_t j = 0; j < current_t - 1; ++j)
                z_keys_buffer_loop.push_back(std::move(y_to_split->_keys[current_t + j]));
            if (!y_was_leaf_when_split)
            {
                z_pointers_buffer_loop.reserve(current_t);
                for (size_t j = 0; j < current_t; ++j)
                    if ((current_t + j) < y_to_split->_pointers.size())
                        z_pointers_buffer_loop.push_back(y_to_split->_pointers[current_t + j]);
            }

            tree_data_type median_key_to_parent = std::move(y_to_split->_keys[current_t - 1]);
            y_final_keys_buffer_loop.reserve(current_t - 1);
            for (size_t j = 0; j < current_t - 1; ++j)
                y_final_keys_buffer_loop.push_back(std::move(y_to_split->_keys[j]));
            if (!y_was_leaf_when_split)
            {
                y_final_pointers_buffer_loop.reserve(current_t);
                for (size_t j = 0; j < current_t; ++j)
                    if (j < y_to_split->_pointers.size())
                        y_final_pointers_buffer_loop.push_back(y_to_split->_pointers[j]);
            }

            z_new_sibling->_keys.assign(std::make_move_iterator(z_keys_buffer_loop.begin()), std::make_move_iterator(z_keys_buffer_loop.end()));
            if (!y_was_leaf_when_split)
                z_new_sibling->_pointers.assign(z_pointers_buffer_loop.begin(), z_pointers_buffer_loop.end());
            y_to_split->_keys.assign(std::make_move_iterator(y_final_keys_buffer_loop.begin()), std::make_move_iterator(y_final_keys_buffer_loop.end()));
            if (!y_was_leaf_when_split)
                y_to_split->_pointers.assign(y_final_pointers_buffer_loop.begin(), y_final_pointers_buffer_loop.end());
            parent_for_split->_pointers.insert(parent_for_split->_pointers.begin() + child_idx_for_split + 1, z_new_sibling);
            parent_for_split->_keys.insert(parent_for_split->_keys.begin() + child_idx_for_split, std::move(median_key_to_parent));

            if (this->compare_keys(parent_for_split->_keys[child_idx_for_split].first, key_copy))
            {
                next_child_idx = child_idx_for_split + 1;
            }
            else
            {
                next_child_idx = child_idx_for_split;
            }
        }
        if (next_child_idx >= current_node->_pointers.size() || current_node->_pointers[next_child_idx] == nullptr)
        {
            if (this->_logger)
                this->_logger->error("Attempting to descend to an invalid child after split/selection (HYBRID loop).");
        }
        current_node_outer_ptr = &(current_node->_pointers[next_child_idx]);
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::insert(tree_data_type &&data)
{
    if (this->_logger)
    {
        this->_logger->trace("B_tree insert(const tree_data_type&) called.");
    }
    tree_data_type data_copy = std::move(data);
    return this->insert(data_copy);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template <typename... Args>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::emplace(Args &&...args)
{
    if (this->_logger)
    {
        this->_logger->trace("B_tree emplace called.");
    }
    tree_data_type temp_data(std::forward<Args>(args)...);
    return this->insert(std::move(temp_data));
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::insert_or_assign(const tree_data_type &data)
{
    if (this->_logger)
        this->_logger->trace("B_tree insert_or_assign(const) called.");
    btree_iterator it = this->find(data.first);
    if (it != this->end())
    {
        it->second = data.second;
        return it;
    }

    else
    {
        return this->insert(data).first;
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::insert_or_assign(tree_data_type &&data)
{
    if (this->_logger)
        this->_logger->trace("B_tree insert_or_assign(move) called.");
    tkey key_to_find = data.first;
    btree_iterator it = this->find(key_to_find);
    if (it != this->end())
    {

        it->second = std::move(data.second);
        return it;
    }

    else
    {
        return this->insert(std::move(data)).first;
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template <typename... Args>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::emplace_or_assign(Args &&...args)
{
    if (this->_logger)
        this->_logger->trace("B_tree emplace_or_assign called.");
    tree_data_type temp_data(std::forward<Args>(args)...);
    return this->insert_or_assign(std::move(temp_data));
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_iterator pos)
{
    if (this->_logger)
        this->_logger->trace("B_tree erase(key) [RECURSIVE] called.");
    if (!this->_root)
    {
        return this->end();
    }

    btree_iterator key_iter = this->find(pos);
    if (key_iter == this->end())
    {
        return this->end();
    }

    size_t initial_size = this->_size;
    erase_recursive(this->_root, pos);
    if (this->_root && this->_root->_keys.empty())
    {

        typename B_tree<tkey, tvalue, compare, t>::btree_node *old_root = this->_root;
        if (old_root->_pointers.empty())
        {
            this->_root = nullptr;
        }
        else
        {
            if (old_root->_pointers.size() == 1)
            {
                this->_root = old_root->_pointers[0];
                old_root->_pointers.clear();
            }
            else
            {
                if (this->_logger)
                    this->_logger->error("Root became empty but has != 1 child after erase.");
                throw std::runtime_error("Root became empty but has != 1 child after erase.");
                old_root = nullptr;
            }
        }
        if (old_root)
        {
            pp_allocator<btree_node> node_alloc = this->get_allocator();
            node_alloc.delete_object(old_root);
        }
    }

    if (this->_size == initial_size && initial_size > 0)
    {
        if (this->_logger)
            this->_logger->warning("Tree size did not decrease after erasing existing key.");
        this->_size--;
    }
    else if (this->_size > initial_size)
    {
        if (this->_logger)
            this->_logger->error("Tree size increased after erase.");
        throw std::runtime_error("Tree size increased after erase.");
    }
    else if (this->_size < initial_size - 1 && initial_size > 0)
    {

        if (this->_logger)
            this->_logger->error("Tree size decreased by more than 1 after erase.");
        throw std::runtime_error("Tree size decreased incorrectly after erase.");
    }

    if (this->_size == 0 && this->_root != nullptr)
    {
        if (this->_logger)
            this->_logger->error("Tree size is 0 but root is not null after erase.");
        pp_allocator<btree_node> node_alloc = this->get_allocator();
        node_alloc.delete_object(this->_root);
        this->_root = nullptr;
    }
    else if (this->_size > 0 && this->_root == nullptr)
    {

        if (this->_logger)
            this->_logger->error("Tree size > 0 but root is null after erase.");
        throw std::runtime_error("Tree size > 0 but root is null after erase.");
    }
    return this->lower_bound(pos);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator pos)
{
    if (this->_logger)
        this->_logger->trace("B_tree erase(const_iterator) delegating to erase(key).");
    if (pos == this->cend())
    {
        if (this->_logger)
            this->_logger->warning("Attempt to erase using cend iterator.");
        return this->end();
    }
    tkey key_to_erase = pos->first;
    return this->erase(key_to_erase);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_iterator beg, btree_iterator en)
{
    if (this->_logger)
        this->_logger->trace("B_tree erase(const_iterator range) delegating to erase(key).");
    std::vector<tkey> keys_to_erase;
    btree_const_iterator current = beg;
    while (current != en)
    {
        if (current == this->cend())
        {
            if (this->_logger)
                this->_logger->warning("erase(range): encountered cend before reaching end iterator 'en'.");
            break;
        }
        keys_to_erase.push_back(current->first);
        ++current;
    }
    tkey end_key;
    bool end_is_valid = (en != this->cend());
    if (end_is_valid)
    {
        end_key = en->first;
    }
    for (const auto &k : keys_to_erase)
    {
        this->erase(k);
    }
    if (end_is_valid)
    {

        return this->lower_bound(end_key);
    }
    else
    {
        return this->end();
    }
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator beg, btree_const_iterator en)
{
    return this->erase(btree_const_iterator(beg), btree_const_iterator(en));
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t_param>
typename B_tree<tkey, tvalue, compare, t_param>::btree_iterator
B_tree<tkey, tvalue, compare, t_param>::erase(const tkey &key)
{
    static constexpr std::size_t t = t_param;
    if (this->_logger)
        this->_logger->trace("B_tree erase(key) [RECURSIVE] called.");
    if (!this->_root)
    {
        return this->end();
    }
    btree_const_iterator key_const_iter = this->find(key);
    if (key_const_iter == this->cend())
    {
        return this->end();
    }

    size_t initial_size = this->_size;
    erase_recursive(this->_root, key);
    if (this->_root && this->_root->_keys.empty())
    {
        typename B_tree<tkey, tvalue, compare, t_param>::btree_node *old_root = this->_root;
        if (old_root->_pointers.empty())
        {
            this->_root = nullptr;
        }
        else
        {
            if (old_root->_pointers.size() == 1)
            {
                this->_root = old_root->_pointers[0];
                old_root->_pointers.clear();
            }
            else
            {
                if (this->_logger)
                    this->_logger->error("Root became empty but has invalid number of children after erase.");
                throw std::runtime_error("Root became empty but has invalid number of children after erase.");
                old_root = nullptr;
            }
        }
        if (old_root)
        {
            pp_allocator<typename B_tree<tkey, tvalue, compare, t_param>::btree_node> node_alloc = this->get_allocator();
            node_alloc.delete_object(old_root);
        }
    }

    if (initial_size > 0)
    {
        this->_size--;
        if (this->_size != initial_size - 1)
        {
            if (this->_logger)
                this->_logger->error("Tree size decreased incorrectly after erase.");
            throw std::runtime_error("Tree size decreased incorrectly after erase.");
        }
        if (this->_size == 0 && this->_root != nullptr)
        {
            if (this->_logger)
                this->_logger->error("Tree size is 0 but root is not null after erase.");
            pp_allocator<typename B_tree<tkey, tvalue, compare, t_param>::btree_node> node_alloc = this->get_allocator();
            node_alloc.delete_object(this->_root);
            this->_root = nullptr;
            throw std::runtime_error("Tree size is 0 but root is not null after erase.");
        }
        else if (this->_size > 0 && this->_root == nullptr)
        {
            if (this->_logger)
                this->_logger->error("Tree size > 0 but root is null after erase.");
            throw std::runtime_error("Tree size > 0 but root is null after erase.");
        }
    }
    else
    {
        if (this->_size != 0)
        {
            if (this->_logger)
                this->_logger->error("Tree size changed after erase on empty tree.");
            this->_size = 0;
        }
    }

    return this->lower_bound(key);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool compare_pairs(const typename B_tree<tkey, tvalue, compare, t>::tree_data_type &lhs,
                   const typename B_tree<tkey, tvalue, compare, t>::tree_data_type &rhs)
{
    compare comp_instance = compare();
    return comp_instance(lhs.first, rhs.first);
}

template <typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool compare_keys(const tkey &lhs, const tkey &rhs)
{
    compare comp_instance = compare();
    return comp_instance(lhs, rhs);
}

#endif