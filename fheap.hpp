#ifndef __FHEAP_H_
#define __FHEAP_H_

#include <iostream>
#include <initializer_list>
#include <optional>
#include <vector>
#include <cmath>
#include <memory>
#include <queue>

template <typename T>
class PriorityQueue {
    public:
        virtual void insert(const T& item) = 0;
        virtual std::optional<T> extract_min() = 0;
        virtual bool is_empty() const = 0;
};

template <typename T>
class FibonacciNode {
    public:
        // Constructors
        FibonacciNode();
        FibonacciNode(const T& item)
            :key(item), degree(0), marked(false), child(nullptr), right(nullptr) {}

        // Destructor
		// You can implement your custom destructor.
        // ~FibonacciNode() = default;
        ~FibonacciNode()
        {
            // std::cout<<"node "<<key<<" destructor called\n";
            right.reset();
            left.reset();
            if(child) child.reset();
            if(parent.lock()) parent.reset();
        }

        T key;
        size_t degree;
		bool marked;

		std::shared_ptr<FibonacciNode<T>> right;
		std::shared_ptr<FibonacciNode<T>> child;
        // NOTE: To prevent circular reference, left/parent pointer should be set to weak_ptr.
        std::weak_ptr<FibonacciNode<T>> left;
        std::weak_ptr<FibonacciNode<T>> parent;
};

template <typename T>
class FibonacciHeap : public PriorityQueue<T> {
    public:
        // Constructors
        FibonacciHeap()
            : min_node(nullptr), size_(0) {}
        FibonacciHeap(const T& item)
            : min_node(nullptr), size_(0) { insert(item); }

        // Disable copy constructor.
        FibonacciHeap(const FibonacciHeap<T> &);

        // Destructor
        ~FibonacciHeap();

        void insert(const T& item) override;
		void insert(std::shared_ptr<FibonacciNode<T>>& node);

        // Return raw pointer of the min_node.
        FibonacciNode<T>* get_min_node() { return min_node.get(); }

        std::optional<T> get_min() const;

        std::optional<T> extract_min() override;

		void decrease_key(std::shared_ptr<FibonacciNode<T>>& x, T new_key);

		void remove(std::shared_ptr<FibonacciNode<T>>& node);

        bool is_empty() const override { return !size_; }

        size_t size() const { return size_; }

        // additional function
        void exchange(std::shared_ptr<FibonacciNode<T>>& x, std::shared_ptr<FibonacciNode<T>>& y)
        {
            std::shared_ptr<FibonacciNode<T>> tmp = x;
            x = y;
            y = tmp;
        }

    private:

        std::shared_ptr<FibonacciNode<T>> min_node;
        size_t size_;

        void consolidate();
        void merge(std::shared_ptr<FibonacciNode<T>>& x, std::shared_ptr<FibonacciNode<T>>& y);
		void cut(std::shared_ptr<FibonacciNode<T>>& x);
		void recursive_cut(std::shared_ptr<FibonacciNode<T>>& x);

};

template <typename T>
FibonacciHeap<T>::~FibonacciHeap() {
	// TODO
	// NOTE: Be aware of memory leak or memory error.
    // std::cout<<"destructor called\n";
    while(min_node) extract_min();
}

template <typename T>
std::optional<T> FibonacciHeap<T>::get_min() const {
	if(!min_node) 
		return std::nullopt;
	else
		return min_node->key;
}

template <typename T>
void FibonacciHeap<T>::insert(const T& item) {
	std::shared_ptr<FibonacciNode<T>> node = std::make_shared<FibonacciNode<T>>(item);
	insert(node);
}

template <typename T>
void FibonacciHeap<T>::insert(std::shared_ptr<FibonacciNode<T>>& node) {
	// TODO
    node->right = node;
    node->left = node;

    if(min_node)
    {
        min_node->left.lock()->right = node;
        node->left = min_node->left;
        min_node->left = node;
        node->right = min_node;

        if(node->key < min_node->key) min_node = node;
    }
    else min_node = node;
    size_++;
}

template <typename T>
std::optional<T> FibonacciHeap<T>::extract_min() {
	// TODO
    std::optional<T> min_key = get_min();
    std::shared_ptr<FibonacciNode<T>> z = min_node;

    if(z!=nullptr)
    {
        if(z->degree)
        {
            std::shared_ptr<FibonacciNode<T>> x = z->child;
            std::vector<std::shared_ptr<FibonacciNode<T>>> childList(z->degree, nullptr);
            std::shared_ptr<FibonacciNode<T>> tmp = x;

            for(int i=0;i<(int)z->degree;i++)
            {
                childList[i] = tmp;
                tmp = tmp->right;
            }
            for(int i=0;i<(int)z->degree;i++)
            {
                x = childList[i];
                min_node->left.lock()->right = x;
                x->left = min_node->left;
                min_node->left = x;
                x->right = min_node;
                x->parent = min_node->parent;
            }
        }
        z->left.lock()->right = z->right;
        z->right->left = z->left;

        if(z==z->right)
        {
            min_node->right.reset();
            min_node->left.reset();
            min_node.reset();
            min_node = nullptr;
        }
        else
        {
            min_node = z->right;
            consolidate();
        }
        size_--;
    }
    return min_key;
}

template <typename T>
void FibonacciHeap<T>::decrease_key(std::shared_ptr<FibonacciNode<T>>& x, T new_key) {
	// TODO
    if(x->key < new_key) return;
    x->key = std::move(new_key);
    
    std::shared_ptr<FibonacciNode<T>> p = x->parent.lock();

    if(p!=nullptr && x->key < p->key)
    {
        cut(x);
        recursive_cut(p);
    }

    if(x->key < min_node->key) min_node = x;
}

template <typename T>
void FibonacciHeap<T>::remove(std::shared_ptr<FibonacciNode<T>>& x) {
	// TODO
	decrease_key(x, std::numeric_limits<T>::min());
    extract_min();
}

template <typename T>
void FibonacciHeap<T>::consolidate() {
	float phi = (1 + sqrt(5)) / 2.0;
	int len = int(log(size_) / log(phi)) + 10;
	// TODO

	std::vector<std::shared_ptr<FibonacciNode<T>>> A(len, nullptr);
    std::vector<std::shared_ptr<FibonacciNode<T>>> rootList;
    std::shared_ptr<FibonacciNode<T>> tmp = min_node;
    do
    {
        rootList.push_back(tmp);
        tmp = tmp->right;
    } while (tmp!=min_node);

    for(int i=0;i<(int)rootList.size();i++)
    {
        tmp = rootList[i];
        std::shared_ptr<FibonacciNode<T>> x = tmp;
        int d = x->degree;
        while(A[d]!=nullptr)
        {
            std::shared_ptr<FibonacciNode<T>> y = A[d];
            if(x->key > y->key) exchange(x,y);
            merge(x,y);
            A[d] = nullptr;
            d++;
        }
        A[d] = x;
    }
    rootList.clear();
    min_node = nullptr;

    for(int i=0;i<len;++i)
    {
        if(A[i]!=nullptr)
        {
            if(min_node==nullptr)
            {
                min_node = A[i];
                min_node->left = min_node;
                min_node->right = min_node;
            }
            else
            {
                min_node->left.lock()->right = A[i];
                A[i]->left = min_node->left;
                min_node->left = A[i];
                A[i]->right = min_node;

                if(A[i]->key < min_node->key) min_node = A[i];
            }
        }
    }
}

template <typename T>
void FibonacciHeap<T>::merge(std::shared_ptr<FibonacciNode<T>>& x, std::shared_ptr<FibonacciNode<T>>& y) {
	// TODO
    
    if(x==nullptr || y==nullptr) return;

    if(x->key > y->key) merge(y,x);
    else
    {
        // x < y

        y->left.lock()->right = y->right;
        y->right->left = y->left;

        if(x->child!=nullptr)
        {
            x->child->left.lock()->right = y;
            y->left = x->child->left;
            x->child->left = y;
            y->right = x->child;
        }
        else
        {
            x->child = y;
            y->right = y;
            y->left = y;
        }

        y->parent = x;
        x->degree++;
        y->marked = false;
    }
}

template <typename T>
void FibonacciHeap<T>::cut(std::shared_ptr<FibonacciNode<T>>& x) {
	// TODO
    if(x->right == x) x->parent.lock()->child = nullptr;
    else
    {
        x->right->left = x->left;
        x->left.lock()->right = x->right;
        if(x->parent.lock()->child == x) x->parent.lock()->child = x->right;
    }
    x->parent.lock()->degree--;

    min_node->right->left = x;
    x->right = min_node->right;
    min_node->right = x;
    x->left = min_node;

    x->parent = min_node->parent;
    x->marked = false;
}

template <typename T>
void FibonacciHeap<T>::recursive_cut(std::shared_ptr<FibonacciNode<T>>& x) {
	// TODO
    std::shared_ptr<FibonacciNode<T>> p = x->parent.lock();
    if(p!=nullptr)
    {
        if(x->marked == false) x->marked = true;
        else
        {
            cut(x);
            recursive_cut(p);
        }
    }
}

#endif // __FHEAP_H_

//valgrind --leak-check=yes ./test
//valgrind --leak-check=yes --leak-check=full --show-leak-kinds=all ./test