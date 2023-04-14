#include <iostream>
#include <cassert>
#include <queue>
#include <stack>
#include <vector>
#include <cstring>
#include <list>
#include <memory>
#include <mutex>
#include <cmath>
#include <set>
#include <string>
#include <algorithm>

struct bin_node_t {
  int value;
  bin_node_t *left;
  bin_node_t *right;
};

bin_node_t *rebuild_bin_tree_recursive(
    int *pre_start, int *pre_end, int *in_start, int *in_end) {
  int root_value = pre_start[0];
  bin_node_t *root = new bin_node_t;
  root->value = root_value;
  root->left = nullptr;
  root->right = nullptr;
  if (pre_start == pre_end) {
    if (*pre_start != *in_start)
      throw std::invalid_argument("invalid input");
    else
      return root;
  }
  int *in_root = in_start;
  while (in_root != in_end) {
    if (*in_root == root_value) break;
    ++in_root;
  }
  if (*in_root != root_value)
    throw std::invalid_argument("invalid input");
  int left_count = in_root - in_start;
  if (left_count > 0) {
    root->left = rebuild_bin_tree_recursive(
        pre_start + 1, pre_start + left_count, in_start, in_root - 1);
  }
  if (left_count < pre_end - pre_start) {
    root->right = rebuild_bin_tree_recursive(
        pre_start + left_count + 1, pre_end, in_root + 1, in_end);
  }
  return root;
}

// 已只前序和中序遍历重构二叉树
bin_node_t *rebuild_bin_tree(int *pre, int *in, int len) {
  if (!pre || !in || len <= 0) return nullptr;
  return rebuild_bin_tree_recursive(pre, pre + len - 1, in, in + len - 1);
}

// 宽度遍历二叉树
void width_print_bin_tree(bin_node_t *root) {
  if (!root) return;
  std::queue<bin_node_t *> queue;
  queue.push(root);
  while(queue.size()) {
    auto node = queue.front();
    printf("%d\t", node->value);
    queue.pop();
    if (node->left) queue.push(node->left);
    if (node->right) queue.push(node->right);
  }
}

// 判断一个序列是不是入栈序列的出栈序列
// 如 12345 其出栈序列为 45321 而 43512 不是出栈序列
bool is_pop_stack(int *push, int *pop, int len) {
  if (!push || !pop || len <= 0) return false;
  bool r{false};
  std::stack<int> stack;
  int *push_next = push;
  int *pop_next = pop;
  while (push_next < push + len) {
    while (stack.empty() || stack.top() != *pop_next) {
      stack.push(*push_next);
      ++push_next;
    }
    if (stack.top() != *pop) return false;
    stack.pop();
    ++pop_next;
  }
  if (stack.empty() && pop_next == pop + len) r = true;
  return r;
}

// 求斐波那契数列的第n项
unsigned int fibonacci(unsigned int n) {
  unsigned int r1[] = {0, 1};
  if (n <= 2) {
    return r1[n - 1];
  }
  unsigned int one = 1;
  unsigned int two = 0;
  unsigned int r{0};
  for (unsigned i = 2; i < n; ++i) {
    r = one + two;
    two = one;
    one = r;
  }
  return r;
}

// 求二叉树的镜像
// 求镜像其实就是左右树的翻转
void mirror(bin_node_t *root) {
  if (!root || (!root->left && !root->right)) return;
  auto temp = root->left;
  root->left = root->right;
  root->right = temp;
  if (root->left) mirror(root->left);
  if (root->right) mirror(root->right);
}

// 寻找二叉树中和等于指定值的路径(从根节点)
void find_path(
    bin_node_t *root, int sum, std::vector<int> &path, int &cur_sum) {
  cur_sum += root->value;
  path.push_back(root->value);
  bool is_leaf = (!root->left) && (!root->right);
  if (cur_sum == sum && is_leaf) {
    printf("found: \n");
    for (auto it = path.begin(); it != path.end(); ++it)
      printf("%d\t", *it);
    // path.clear(); 可以不用，返回出栈的时候该路径的所有节点都被清除了
    printf("\n");
  }
  if (root->left) find_path(root, sum, path, cur_sum);
  if (root->right) find_path(root, sum, path, cur_sum);

  // 记住返回时删除当前节点
  cur_sum -= root->value;
  path.pop_back();
}

void find_path(bin_node_t *root, int sum) {
  if (!root) return;
  int cur_sum{0};
  std::vector<int> path;
  find_path(root, sum, path, cur_sum);
}

struct list_node_t {
  int value;
  list_node_t *next;
  list_node_t *sibling;
};

void clone_gen_node(list_node_t *head) {
  auto node = head;
  while (head) {
    auto new_node = new list_node_t;
    new_node->value = node->value;
    new_node->next = node->next;
    new_node->sibling = nullptr;

    auto next = node->next;
    head->next = new_node;
    head = next;
  }
}

void clone_gen_sibling(list_node_t *head) {
  while (head) {
    if (head->sibling) {
      if (head->next) {
        head->next->sibling = head->sibling->next;
      }
    }
    head = head->next;
  }
}

list_node_t *clone_split(list_node_t *head) {
  if (!head || !head->next) return nullptr;
  list_node_t *node = head;
  list_node_t *cloned = head->next;
  list_node_t *cloned_node = head->next;
  
  node = cloned_node->next;
  head->next = node;

  while (node != nullptr) {
    cloned_node->next = node->next;
    cloned_node = cloned_node->next;
    node->next = cloned_node ? cloned_node->next : nullptr;
    node = node->next;
  }

  return cloned;
}

// 复制复杂链表
// 第一步将所有新的节点位于之前的后面
// 第二步将所有随机节点分配处理
// 第三部拆分两个链表
list_node_t *clone(list_node_t *head) {
  if (!head) return nullptr;
  clone_gen_node(head);
  clone_gen_sibling(head);
  return clone_split(head);
}

void trans_to_list(bin_node_t *root, bin_node_t **last_of_list) {
  if (!root) return;
  auto cur = root;
  if (root->left)
    trans_to_list(root->left, last_of_list);
  cur->left = *last_of_list;
  if (*last_of_list)
    (*last_of_list)->right = cur;
  *last_of_list = cur;
  if (cur->right)
    trans_to_list(root->right, last_of_list);
}

// 将二叉树转换为双向链表
bin_node_t *trans_to_list(bin_node_t *root) {
  if (!root || (!root->left && !root->right)) return root;
  bin_node_t *last_of_list = nullptr;
  trans_to_list(root, &last_of_list);
  bin_node_t *r{last_of_list};
  while (r->left)
    r = r->left;
  return r;
}

// 翻转字符串 i come from chengdu. to chengdu. from come i.
void turn_str(char *str) {
  if (!str) return;
  int len = strlen(str);
  if (len <= 1) return;
  int i = 0, j = 0;
  // 第一步翻转整个字符串
  j = len - 1;
  while (i < j) {
    char temp = str[i];
    str[i] = str[j];
    str[j] = temp;
    ++i;
    --j;
  }
  // 第二部翻转单词(需排除空格)
  i = 0;
  int start = 0, end = 0;
  while (str[i]) {
    while (str[i] && str[i] != ' ') {
      ++i;
    }
    start = i;
    end = i - 1;
    if (start < end) {
      while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        ++start;
        --end;
      }
    }
  }
  if (str[i]) ++i;
}

// 求字符串所有可能出现的组合 如 123 132 213 231 321 312
// 全排列
void permutation(char *str, char *begin) {
  if ('\0' == *begin) {
    printf("%s\n", str);
    return;
  }
  for (char *ch = begin; *ch != '\0'; ++ch) {
    char temp = *ch;
    *ch = *begin;
    *begin = temp;

    permutation(str, begin + 1);

    temp = *ch;
    *ch = *begin;
    *begin = temp;
  }
}

void permutation(char *str) {
  if (!str) return;
  permutation(str, str);
}

// 二分查找中值
int partition(int *array, int low, int high) {
  if (!array || low < high) return 0;
  int pivot = array[low];
  while (low < high) {
    while (low < high && array[high] > pivot) {
      --high;
    }
    if (low < high)
      array[low++] = array[high];
    while (low < high && array[low] < pivot) {
      ++low;
    }
    if (low < high)
      array[high--] = array[low];
  }
  array[low] = pivot;
  return low;
}

void quick_sort(int *array, int low, int high) {
  if (!array || low >= high) return;
  int mid = partition(array, low, high);
  if (mid > low + 1) quick_sort(array, low, mid - 1);
  if (mid < high - 1) quick_sort(array, mid + 1, high);
}

bool g_invalid_input{false};

// 求有正有负的数组子序列中最大的和(全正，或者全负的没有意义)
int find_greatest_sum(int *nums, int len) {
  if (!nums || len <= 0) {
    g_invalid_input = true;
    return 0;
  }
  g_invalid_input = false;
  int r{0};
  int cur_sum{0};
  for (int i = 0; i < len - 1; ++i) {
    if (cur_sum <= 0)
      cur_sum = nums[i];
    else
      cur_sum += nums[i];
    if (cur_sum > r) r = cur_sum;
  }
  return r;
}

const int g_max_number_len{10};
char g_str_combine1[g_max_number_len * 2 + 1]{0,};
char g_str_combine2[g_max_number_len * 2 + 1]{0,};

int combine(const void *str_num1, const void *str_num2) {
  strcpy(g_str_combine1, *(const char **)str_num1);
  strcpy(g_str_combine1, *(const char **)str_num2);
 
  strcpy(g_str_combine2, *(const char **)str_num2);
  strcpy(g_str_combine2, *(const char **)str_num1);

  return strcmp(g_str_combine1, g_str_combine2);
}

// 打印数字所有数组成的最小数
// 组成字符串数据，利用快速排序，将两个数前后和后前比较小的排到前面(combine)
void print_min_number(int *nums, int len) {
  if (!nums || len <= 0) return;
  char **strNum = (char **)(new int[len]); // 不好理解的一处?
  for (int i = 0; i < len; ++i) {
    strNum[i] = new char[g_max_number_len + 1];
    snprintf(strNum[i], g_max_number_len, "%d", nums[i]);
  }

  qsort(strNum, len, sizeof(char *), combine);
  for (int i = 0; i < len; ++i)
    printf("%s", strNum[i]);

  for (int i = 0; i < len; ++i)
    delete[] strNum[i];

  delete[] strNum;
}

// 查找首次出现的不重复的字符
// 使用int数组组成一个简单的哈希[ascii中256大小足够容纳]
char find_first_not_repeat_char(char *str, int len) {
  if (!str || len <= 0) return '\0';
  int hash[256]{0,};
  for (int i = 0; i < 256; ++i) {
    hash[i] = 0;
  }
  auto p = str;
  while (*p) { hash[(int)*p++]++; }
  p = str;
  while (*p) { if (1 == hash[(int)*p++]) { return *(p - 1); } }
  return '\0';
}

int min_in_order(int *nums, int start, int end) {
  int r = nums[start];
  for (int i = start + 1; i < end; ++i) {
    if (r > nums[start])
      r = nums[start];
  }
  return r;
}

bool is_ugly(int num) {
  while (num % 2) 
    num = num / 2;
  while (num % 3)
    num = num / 3;
  while (num % 5)
    num = num / 5;

  return 1 == num;
}

// 获得丑数
int get_ugly(int index) {
  int founded{0};
  int num{0};
  while (founded < index) {
    ++num;
    if (is_ugly(num)) ++founded;
  }
  return num;
}

// 判断一个列表是否是列表的子列表
bool is_sub_list(int *list, int len, int *list1, int len1) {
  if (!list || len <= 0 || !list1 || len1 <= 0 || len1 > len) return false;
  int i = 0;
  int j = len1 - 1;
  while (list[i] == list1[i] && list[j] == list1[j]) {
    ++i;
    --j;
    if (i >= j) break;
  }
  bool r = i >= j;
  if (!r && len1 < len) {
    r = is_sub_list(list + 1, len - 1, list1, len1);
  }
  return r;
}

// 查找旋转数组中最小的数 如 45123最小的为1
int min_in_array(int *nums, int len) {
  if (!nums || len <= 0) {
    g_invalid_input = true;
    return 0;
  }
  g_invalid_input = false;
  int mid = 0;
  int index1 = 0;
  int index2 = len - 1;
  while (nums[index1] >= nums[index2]) {
    if (index2 - index1 == 1) {
      mid = index2;
      break;
    }
    mid = (index1 + index2) / 2;

    if (nums[index1] == nums[mid] && nums[index1] == nums[index2]) {
      return min_in_order(nums, index1, index2);
    }

    if (nums[mid] >= nums[index1])
      index1 = mid;
    else if (nums[mid] <= nums[index2])
      index2 = mid;
  }

  return nums[mid];
}

int get_node_list_len(list_node_t *head) {
  if (!head) return 0;
  int r{1};
  while (head->next) {
    ++r;
    head = head->next;
  }
  return r;
}

// 求两个链表的公共节点
list_node_t *get_public(list_node_t *head1, list_node_t *head2) {
  if (!head1 || !head2) return nullptr;
  if (head1 == head2) return head1;
  int head1_len = get_node_list_len(head1), head2_len = get_node_list_len(head2);
  int step = head1_len - head2_len;
  // 将最长的链表向后移动相差的步数
  if (step < 0) {
    while (step != 0) {
      ++step;
      head2 = head2->next;
    }
  } else {
    while (step != 0) {
      --step;
      head2 = head2->next;
    }
  }
  while (head1 != head2) { // 这时候两个链表剩余部分是一样长，可以不判断头空情况
    head1 = head1->next;
    head2 = head2->next;
  }
  return head1;
}

// 求树的深度
int tree_deep(bin_node_t *root) {
  if (!root) return 0;
  int left = tree_deep(root->left);
  int right = tree_deep(root->right);
  return left > right ? (left + 1) : (right + 1);
}

// 判断一棵树是不是平衡二叉树
bool is_balanced(bin_node_t *root) {
  if (!root) return true;
  int left = tree_deep(root->left);
  int right = tree_deep(root->right);
  int diff = left - right;
  if (diff > 1 || diff < -1) return false;
  return is_balanced(root->left) && is_balanced(root->right);
}

// 判断一棵树是否为平衡二叉树的遍历一次方法
bool is_balanced(bin_node_t *root, int *depth) {
  if (!root) {
    *depth = 0;
    return true;
  }
  int left = 0, right = 0;
  if (is_balanced(root->left, &left) && is_balanced(root->right, &right)) {
    int diff = left - right;
    if (-1 <= diff && diff <= 1) {
      *depth = 1 + (left > right ? left : right);
      return true;
    }
  }
  return false;
}

bool is_sort_by_asc(int *nums, int n) {
  if (!nums || n <= 0) return false;
  if (1 == n) return true;
  if (2 == n) return nums[n - 1] > nums[n - 2];
  return is_sort_by_asc(nums, n - 1) && (nums[n - 1] > nums[n - 2]);
}

void swap_no_temp(int &a, int &b) {
  a ^= b;
  b ^= a;
  a ^= b;
}

// 不用+求和
int add(int a, int b) {
  if (0 == b) return a;
  int sum, carry;
  sum = a ^ b;
  carry = (a & b) << 1;
  return add(sum, carry);
}

// 二分查找
int mid_find(int *nums, int n, int value) {
  if (!nums || n <= 0) return - 1;
  int i = 0, j = n, mid = n / 2;
  while (i < j) {
    if (nums[mid] < value) {
      i = mid;
      mid = (j + mid) / 2;
    } else if (nums[mid] > value) {
      j = mid;
      mid = (i + mid) / 2;
    } else {
      return mid;
    }
  }
  return -1;
}

// 是否为回文
// 回文：从前向后 或从后向前的字符串是一样
bool is_palindrome(char *str) {
  if (!str || '\0' == *str) return false;
  char *p = str;
  while (*p != '\0') ++p;
  p = p - 1;
  while (*str == *p && str < p) {
    ++str;
    --p;
  }
  return str >= p;
}

void merge_sort_recursive(int *nums, int *copy, int start, int end) {
  if (start >= end) return;
  // int mid = (start + end) >> 1;
  int mid = ((end - start) >> 1) + start; // 这个更好理解点,长度一半加上start
  merge_sort_recursive(nums, copy, start, mid);
  merge_sort_recursive(nums, copy, mid + 1, end);
  int start1 = start, end1 =  mid;
  int start2 = mid + 1, end2 = end;
  int copy_index{start};
  while (start1 <= end1 && start2 <= end2)
    copy[copy_index++] = 
      nums[start1] < nums[start2] ? nums[start1++] : nums[start2++];
  for (; start1 <= end1; ++start1)
    copy[copy_index++] = nums[start1];
  for (; start2 <= end2; ++start2)
    copy[copy_index++] = nums[start2];
  // 此时可以进行序列的替换了，实际上放到外面最后进行替换也可以
  /*
  for (int i = start; i <= end; ++i) {
    nums[i] = copy[i];
  }
  */
}

// 归并排序
// 其思想是将所有数字先分为单个数组然后进行俩俩合并
// 大致是：拆分->1->合并(排序)
void merge_sort(int *nums, const int len) {
  if (!nums || len <= 0) return;
  int copy[len];
  merge_sort_recursive(nums, copy, 0, len - 1);
  for (int i = 0; i < len; ++i)
    nums[i] = copy[i];
}

// 求数字数组中的逆序对是数量(使用归并排序)
int inverse_pairs_recursive(int *nums, int *copy, int start, int end) {
  if (start >= end) {
    copy[start] = nums[start];
    return 0;
  }
  int len = (end - start) >> 1;
  int start1 = start, end1 = start + len;
  int start2 = end1 + 1, end2 = end;
  int left = inverse_pairs_recursive(nums, copy, start1, end1);
  int right = inverse_pairs_recursive(nums, copy, start2, end2);
  int index_copy = end;
  int count{0};

  while (start1 <= end1 && start2 <= end2) {
    if (nums[end1] < nums[end2]) {
      copy[index_copy--] = nums[end2--];
      count += end2 - start2 + 1; // 求剩余数组的个数
    } else {
      copy[index_copy--] = nums[end2--];
    }
  }

  while (end1 >= start1) {
    copy[index_copy--] = nums[end1--];
  }
  while (end2 >= start2)
    copy[index_copy--] = nums[end2--];

  return count + left + right;
}

int inverse_pairs(int *nums, const int len) {
  int copy[len];
  for (int i = 0; i < len; ++i)
    copy[i] = 0;
  return inverse_pairs_recursive(nums, copy, 0, len - 1);
}

void print_seq(int start, int end) {
  for (int i = start; i <= end; ++i)
    printf("%d\t", i);
  printf("\n");
}

// 打印和为指定值的所有序列(该序列从1开始到sum)
void print_sum_seq(int sum) {
  if (sum < 3) return;
  int small = 1;
  int big = 2;
  int mid = (sum + 1) >> 1;
  int cur_sum = small + big;
  while (small < mid) {
    if (cur_sum == sum) print_seq(small, big);
    while (small < mid && sum < cur_sum) {
      cur_sum -= small;
      ++small;
      if (cur_sum == sum) print_seq(small, big);
    }
    ++big;
    cur_sum += big;
  }
}

int poke_compare(const void *arg1, const void *arg2) {
  return *(int *)arg1 - *(int *)arg2;
}

// 判断扑克是否为顺子（0为小王，可以补充序列）
bool is_poke_continuous(int *nums, int len) {
  if (!nums || len <= 1) return false;
  qsort(nums, len, sizeof(int), poke_compare);
  int zero_num{0};
  for (int i = 0; i < len; ++i) {
    if (0 == nums[i])
      ++zero_num;
    else
      break;
  }
  int small = zero_num;
  int big = zero_num + 1;
  int diff{0};
  while (big < len) {
    if (nums[small] == nums[big])
      return false;
    diff += nums[big] - nums[small] - 1;
    small = big;
    ++big;
  }
  return diff <= zero_num;
}

// 求序列中最后一个数字
// 这个是数学公式，也可以使用环形链表
unsigned int last_remaining(unsigned int n, unsigned int m) {
  if (n < 1 || m < 1) return -1;
  unsigned int last{0};
  for (unsigned int i = 2; i <= n; ++i)
    last = (last + m) % n;
  return last;
}

unsigned int sum_func(unsigned int) {
  return 0;
}

typedef unsigned int (*func)(unsigned int);

// 求解1-n的和，利用函数指针
unsigned int sum_func_ptr(unsigned int n) {
  func func_list[2] = {sum_func, sum_func_ptr};
  return n + func_list[!!n](n - 1);
}

// 利用模板函数求1+n的和
template <unsigned int N>
struct sum_template {
  enum {n = sum_template<N - 1>::n + N};
};

template <>
struct sum_template<1> {
  enum {n = 1};
};

// 循环方式求和
int add1(int a, int b) {
  int sum{0}, carry{0};
  do {
    sum = a ^ b;
    carry = (a & b) << 1;
    a = sum;
    b = carry;
  } while (b != 0);
  return a;
}

//一棵普通树的结构
struct n_bin_node_t {
  int value;
  std::vector<n_bin_node_t *> child;
};

// 获得从根节点到节点的路径并保存到链表中
bool find_path(
    n_bin_node_t *root, n_bin_node_t *node, std::list<n_bin_node_t *> &r) {
  if (!node) return false;
  bool found{false};
  auto it = root->child.begin();
  while (!found && it != root->child.end()) {
    found = find_path(*it, node, r);
    ++it;
  }
  if (!found && !r.empty()) r.pop_back();
  return found;
}

n_bin_node_t *get_common_node(
    std::list<n_bin_node_t *> &path1, std::list<n_bin_node_t *> &path2) {
  auto it1 = path1.begin();
  auto it2 = path2.begin();
  n_bin_node_t *r{nullptr};
  while (it1 != path1.end() && it2 != path2.end()) {
    if (*it1 == *it2) r = *it1;
    ++it1;
    ++it2;
  }
  return r;
}

// 获得最低公共节点(普通树)
// 普通思路：如果有子树包含两个节点则查找该子树，否则该节点就是所求公共节点,
//           时间复杂度为O(n^2)
// 实现思路：先查找两个节点的路径 -> 转换为求两条链表的最后公共节点
//           查找路径的时间复杂度为O(n)
// 如果是二叉搜索树可以：如果根节点大于两个节点则查找左子树，如果根节点小于两个
// 子节点则查找右子树 直到不满足这两个条件位置的根节点为止(
// 该根节点就是最低公共祖先)
n_bin_node_t *get_last_common_node(
    n_bin_node_t *root, n_bin_node_t *node1, n_bin_node_t *node2) {
  if (!root || !node1 || !node2) return nullptr;
  
  std::list<n_bin_node_t *> path1;
  find_path(root, node1, path1);

  std::list<n_bin_node_t *> path2;
  find_path(root, node2, path2);

  return get_common_node(path1, path2);
}

// 将空格替换为%20 原字符串可以容纳
char *replace_blank(char *str, int size) {
  if (!str || size <= 0) return str;
  int blank_count{0};
  char *p = str;
  int len{0};
  while (*p != '\0') {
    if (' ' == *p) ++blank_count;
    ++p;
    ++len;
  }
  if (blank_count <= 0) return str;
  int need_count = 2 * blank_count;
  if (size <= need_count + len) return str;
  p -= 1;
  char *p1 = p + need_count;
  while (p > str && p1 > p) {
    if ('\0' == *p) {
      *p1++ = '0';
      *p1++ = '2';
      *p1++ = '%';
      ++p;
    } else {
      *p1++ = *p++;
    }
  }
  return str;
}

// 反向输出链表
void reverse_print(list_node_t *head) {
  if (!head) return;
  if (head->next) 
    reverse_print(head->next);
  printf("%d\t", head->value);
  /*
  std::stack<list_node_t *> stack;
  while (head->next) {
    stack.push(head);
    head = head->next;
  }
  stack.push(head); // 注意while条件没有加入最后一个
  while (!stack.empty()) {
    auto node = stack.top();
    printf("%d\t", node->value);
    stack.pop();
  }
  */
}

// 用栈实现队列
template <typename T>
class my_queue {

 public:

   void pop(T value) {
     if (stack2_.empty() && stack1_.empty()) return;
     if (!stack2_.empty()) stack2_.pop();
     while (!stack1_.empty()) {
       stack2_.push(stack1_.top());
       stack1_.pop();
     }
     stack2_.pop();
   }

   void push(T value) {
     stack1_.push(value);
   }

   T top(T value) {
     if (stack2_.empty() && stack1_.empty()) return stack2_.top();
     if (!stack2_.empty()) return stack2_.top();
     while (!stack1_.empty()) {
       stack2_.push(stack1_.top());
       stack1_.pop();
     }
     return stack2_.top();
   }

 private:
   std::stack<T> stack1_;
   std::stack<T> stack2_;

};

// 利用两个队列实现栈
template <typename T>
class my_stack {

 public:
   void push(T value) { // 还有一种方法在出栈时做类似处理即可
     if (queue1_.empty() && queue2_.empty()) queue1_.push(value);
     if (queue1_.empty()) {
       while (!queue2_.empty()) {
         queue1_.push(queue2_.top());
         queue2_.pop();
       }
       queue1_.push(value);
     } else {
       while (!queue1_.empty()) {
         queue2_.push(queue1_.top());
         queue1_.pop();
       }
       queue2_.push(value);
     }
   }

   void pop() {
     if (queue1_.empty() && queue2_.empty()) return;
     if (queue1_.empty())
       queue2_.pop();
     else
       queue1_.pop();
   }

   T top() {
     if (queue1_.empty() && queue2_.empty()) return queue1_.top();
     if (queue1_.empty())
       return queue2_.top();
     else
       return queue1_.top();
   }

 private:
   std::queue<T> queue1_;
   std::queue<T> queue2_;
};

// 查找有序数组中的某个值
// 利用二分查找
int mid_search(int *array, int len, int value) {
  if (!array || len <= 0) return -1;
  int start = 0, end = len, mid = len >> 1;
  while (start < end) {
    if (array[mid] == value) {
      return mid;
    } else if (array[mid] > value) {
      end = mid;
    } else {
      start = mid;  
    }
    mid = (start + end) >> 1;
  }
  return -1;
}

int min_order(int *nums, int start, int end) {
  int r = nums[start];
  for (int i = start; i <= end; ++i) {
    if (nums[i] < r)
      r = nums[i];
  }
  return r;
}

// 旋转数字中的最小值456123
int turn_min(int *nums, int len) {
  if (!nums || len < 1) {
    g_invalid_input = true;
    return 0;
  }
  g_invalid_input = false;
  int i = 0, j = len - 1;
  int mid = (len - 1) >> 1;
  while (i < j) {
    if (j - i == 1) {
      mid = j;
      break;
    }
    if (nums[mid] == nums[i] && nums[i] == nums[j]) {
      return min_order(nums, i, j);
    }
    if (nums[mid] >= nums[i]) {
      i = mid + 1;
    } else if (nums[mid] <= nums[j]) {
      j = mid - 1;
    }
    mid = (i + j) >> 1;
  }
  return nums[mid];
}

// 数字位中含有1的数量
int count_bit1(int num) {
  int r{0};
  while (num) {
    num &= (num - 1);
    ++r;
  }
  return r;
}

bool equal(double a, double b) {
  auto diff = a - b;
  return diff > -0.0000001 && diff < 0.0000001;
}

double power_core(double base, unsigned int n) {
  if (0 == n) return 1;
  if (1 == n) return base;
  int r = power_core(base, n >> 1);
  r *= r;
  if ((n & 0x1) == 1) r *=base;
  return r;
}

// 实现power(一个double数的N次方)
double power(double base, int n) {
  if (equal(base, 0.0) && n < 0) {
    g_invalid_input = true;
    return 0.0;
  }
  g_invalid_input = false;
  unsigned int n_abs = (unsigned int)n;
  if (n < 0) n_abs = (unsigned int)-n;
  auto r = power_core(base, n_abs);
  if (n < 0) r = 1 / r;
  return r;
}

void print_num(char *num) {
  if (!num || '\0' == *num) return;
  bool found_not_zero{false};
  while (*num) {
    if (!found_not_zero && *num != '0')
      found_not_zero = true;
    if (found_not_zero)
      printf("%c", *num);
    ++num;
  }
  printf("\n");
}

void print_num_recursive(char *num, int n, int index) {
  if (index + 1 == n) {
    print_num(num);
    return;
  }
  for (int i = 0; i < 10; ++i) {
    num[index + 1] = i + '0';
    print_num_recursive(num, n, index + 1);
  }
}

// 打印1-n的所有数
void print_num(int n) {
  if (n <= 0) return;
  char *str = new char[n + 1];
  memset(str, 0, n + 1); 
  str[n] = '\0';
  for (int i = 0; i < 10; ++i) {
    str[0] = i + '0';
    print_num_recursive(str, n, 0);
  }

  delete[] str;
}

// 删除一个链表中的节点
void del_list_node(list_node_t **head, list_node_t *node) {
  if (!head || !*head || !node) return;
  if (*head == node) {
    delete node;
    node = nullptr;
    *head = nullptr;
  } else if (node->next) {
    auto next_node = node->next;
    node->value = next_node->value;
    node->next = next_node->next;
    delete next_node;
    next_node = nullptr;
  } else { // 如果位于尾结点则需要遍历整个链表删除
    auto cur = *head;
    while (cur && cur->next != node) {
      cur = cur->next;
    }
    if (!cur) return;
    cur->next = nullptr;
    delete node;
    node = nullptr;
  }
}

bool is_even(int n) {
  return 1 == (n & 0x1);
}

void sort_list_by_func(int *nums, int len, bool (*func)(int)) {
  if (!nums || len <= 0 || !func) return;
  int i = 0, j = len - 1;
  while (i < j) {
    while (i < j && func(nums[j])) --j;
    while (i < j && !func(nums[i])) ++i;
    if (i < j) {
      int temp = nums[i];
      nums[i] = nums[j];
      nums[j] = temp;
    }
  }
}

// 将列表中的奇数排在偶数前
void sort_list_by_even(int *nums, int len) {
  if (!nums || len <= 0) return;
  sort_list_by_func(nums, len, is_even);
}

// 查找链表中倒数第k个节点
list_node_t *find_last_k_node(list_node_t *head, int k) {
  if (!head || k <= 0) return nullptr;
  auto node1 = head;
  int step = k - 1;
  while (step > 0 && node1) {
    node1 = node1->next;
    --step;
  }
  if (step > 0) return nullptr;
  auto node2 = head;
  while (node1->next) {
    node1 = node1->next;
    node2 = node2->next;
  }
  return node2;
}

// 链表中间节点
// 两个指针都指向头
// 第一个走两步 第二个走一步 当第一个走向末尾时第二个就是结果
list_node_t *find_middle_node(list_node_t *head) {
  if (!head) return nullptr;
  auto node1 = head;
  auto node2 = head;
  while (node1 && node1->next) {
    node1 = node1->next ? node1->next->next : nullptr;
    node2 = node2->next;
  }
  return node2;
}

// 链表是否是环形列表
// 个指针都指向头
// 一个走两步 第二个走一步 当第一个赶上第二个的时候就是环形列表
bool is_round_list(list_node_t *head) {
  if (!head) return false;
  auto node1 = head;
  auto node2 = head;
  while (node1 && node1->next) {
    node1 = node1->next ? node1->next->next : nullptr;
    node2 = node2->next;
    if (node1 == node2) return true;
  }
  return false;
}

// 翻转链表
list_node_t *turn_node_list(list_node_t *head) {
  if (!head) return nullptr;
  auto node1 = head;
  auto node2 = head->next;
  list_node_t *node = nullptr;
  while (node2) {
    node = node2->next;
    node2->next = node1;

    node1 = node2;
    node2 = node;
  }
  head->next = nullptr;
  return node1;
}

// 合并两个从小到大的有序链表
list_node_t *merge_node_list(list_node_t *head1, list_node_t *head2) {
  if (!head2) 
    return head1;
  else if (!head1)
    return head2;
  auto r = head1;
  if (head1->value > head2->value) {
    r = head2;
    r->next = merge_node_list(head2->next, head1);
  } else {
    r->next = merge_node_list(head1->next, head2);
  }
  return r;
}

bool has_sub_tree(bin_node_t *root1, bin_node_t *root2) {
  if (!root2) return true;
  if (root1->value != root2->value) return false;
  return has_sub_tree(root1->left, root2->left) &&
    has_sub_tree(root1->right, root2->right);
}

// 判断一个树是否是子树
bool is_sub_tree(bin_node_t *root1, bin_node_t *root2) {
  if (!root1 || !root2) return false;
  bool r{false};
  if (root1->value == root2->value)
    r = has_sub_tree(root1, root2);
  if (!r)
    r = is_sub_tree(root1->left, root2);
  if (!r)
    r = is_sub_tree(root1->right, root2);
  return r;
}

void print_matrix_cricle(int **nums, int column, int row, int start) {
  int x = start, y = start;
  int x1 = column - start - 1, y1 = row - start - 1;
  for (int i = x; i <= x1; ++i)
    printf("%d\t", nums[x][i]);
  if (x1 > x) {
    for (int i = y; i <= y1; ++i)
      printf("%d\t", nums[i][x1]);
  }
  if (y1 > y && x1 > x) {
    for (int i = x1 - 1; i >= x; --i)
      printf("%d\t", nums[y1][i]);
  }
  if (y1 > y){
    for (int i = y1 - 1; i >= y; --i)
      printf("%d\t", nums[i][x]);
  }
}

// 顺时针打印螺旋矩阵(左上角的点为0,0)
void print_matrix_clockwisely(int **nums, int column, int row) {
  if (!nums || column <= 0 || row <= 0) return;
  int start{0};
  while (column > (start << 1) && row > (start << 1)) {
    print_matrix_cricle(nums, column, row, start);
    ++start;
  }
}

// 带有min函数的栈
template <typename T>
class my_stack_ex {
 public:
   void pop() {
     if (stack2_.empty()) return;
     stack1_.pop();
     stack2_.pop();
   }

   void push(T value) {
     if (stack1_.empty()) {
       stack1_.push(value);
     } else {
       stack1_.push(value < stack1_.top() ? value : stack1_.top());
     }
     stack2_.push(value);
   }

   T min() {
    return stack1_.top();
   }

 private:
   std::stack<T> stack1_;
   std::stack<T> stack2_;
};

// 根据入栈数据判断出栈数据是否合法
bool is_pop_order(int *push, int *pop, int len) {
  if (!push || !pop || len <= 0) return false;
  int *push_end = push + len - 1;
  std::stack<int> stack;
  stack.push(*push);
  while (push < push_end) {
    while (push < push_end && (stack.empty() || stack.top() != *pop))
      stack.push(*push++);
    if (stack.top() != *pop) return false;
    ++pop;
    stack.pop();
  }
  if (push == push_end && stack.empty()) return true;
  return false;
}

// 判断一个序列是否是搜索二叉树的后序遍历结果
bool is_back_order(int *nums, int len) {
  if (!nums || len <= 0) return false;
  int root_value = nums[len - 1];
  int i = 0;
  while (root_value > nums[i] && i < len) ++i;
  for (int j = i; j < len - 1; ++j) { // 判断条件右子树的值都需要比根大
    if (nums[j] < root_value)
      return false;
  }
  bool r{false};
  if (i > 1)
    r = is_back_order(nums, i);
  if (r && len - i - 1 > 1)
    r = is_back_order(nums + i, len - i - 1);
  return r;   
}

void print_way(
    bin_node_t *root, int sum, int &cur_sum, std::vector<int> &path) {
  if (!root) {
    return;
  }
  path.push_back(root->value);
  cur_sum += root->value;
  bool is_leaf = !root->left && !root->right;
  if (cur_sum == sum && is_leaf) {
    for (auto it = path.begin(); it != path.end(); ++it)
      printf("%d\t", *it);
    printf("\n");
  }
  if (root->left)
    print_way(root->left, sum, cur_sum, path);
  if (root->right)
    print_way(root->right, sum, cur_sum, path);
  cur_sum -= root->value;
  path.pop_back();
}

// 打印和为指定值的树路径
void print_way(bin_node_t *root, int sum) {
  if (!root) return;
  std::vector<int> path;
  int cur_sum{0};
  print_way(root, sum, cur_sum, path);
}

// 模板求和
template <unsigned int N>
class SumClass {
 public:
   enum {n = SumClass<N - 1>::n + N};
};

template <>
class SumClass<1>
{
 public:
   enum {n = 1};
};

// 获得数字中bit为1的索引
int get_first_bit_1(int n) {
  int r{0};
  while ((n & 0x1) != 0x1 && r < 8 * (int)sizeof(n)) {
    n = n >> 1;
    ++r;
  }
  return r;
}

const int g_rand_max_number{6};

void probability(int n, int cur, int sum, int *times_list) {
  if (1 == cur) {
    times_list[sum - n]++;
  } else {
    for (int i = 1; i <= g_rand_max_number; ++i)
      probability(n, cur - 1, i + sum, times_list);
  }
}

void probability(int n, int *times_list) {
  for (int i = 1; i <= g_rand_max_number; ++i)
    probability(n, n, i, times_list);
}

// 打印指定n个骰子和的概率
void print_probabitily(int n) {
  if (n <= 0) return;
  int sum_count = g_rand_max_number * n - n + 1;
  int *times_list = new int[sum_count];
  for (int i = 0; i < sum_count; ++i)
    times_list[i] = 0;
  double total_times = power(g_rand_max_number, n);
  
  probability(n, times_list);
  for (int i = 0; i < sum_count; ++i) {
    double rate = times_list[i] / total_times;
    printf("sum: %d, rate: %f\n", i + g_rand_max_number, rate);
  }

  delete[] times_list;
}

// 评委打分计算
int calc_score(int scores[], int jude_types[], int len) {
  double sum1{0.0}, sum2{0.0};
  int count1{0}, count2{0};
  for (int i = 0; i < len; ++i) {
    switch (jude_types[i]) {
      case 1:
        sum1 += scores[i];
        break;
      case 2:
        sum2 += scores[i];
        break;
      default:
        break;
    }
  }
  int avg1 = count1 > 0 ? sum1 / count1 : 0;
  int avg2 = count2 > 0 ? sum2 / count2 : 0;
  int r = avg1 * 0.6 + avg2 * 0.4;
  return r;
}

class Mystr {

 public:
   Mystr(char *str) {
     if (!str) return;
     data_ = new char[strlen(str)];
     strcpy(data_, str);
   }

   ~Mystr() {
     if (data_) {
       delete [] data_;
       data_ = nullptr;
     }
   }

   Mystr & operator = (const Mystr &obj) {
     if (&obj == this) return *this;
     Mystr temp(obj);
     auto t_data = temp.data_;
     temp.data_ = data_;
     data_ = t_data;
     return *this;
   }

   friend std::ostream &operator << (std::ostream &cout, const Mystr &str);

 private:
   char *data_;

};

// 友元重载
std::ostream &operator << (std::ostream &out, const Mystr &str) {
  out << str.data_;
  return out;
}

// 翻转链表
list_node_t *reverse(list_node_t *head) {
  if (!head) return nullptr;
  auto node1 = head;
  auto node2 = head->next;
  list_node_t *temp = nullptr;
  while (node2) {
    temp = node2->next;
    node2->next = node1;
    node1 = node2;
    node2 = temp ? temp->next : nullptr;
  }
  head->next = nullptr;
  return node1;
}

// 上千万条数据中找出其中重复最高的十条
// 1、建立一个简单的hash table 取头尾的字符和中间的两个字符，如果这四个字符相同
// 则高度疑似，则开始比较，相同的将hash值加1
// 2、对hash table 处理找出次数最多的十条

// 查找最后可能超过出现一半的数字
int find_repeat_more_than_half(int *nums, int len) {
  if (!nums || len <= 0) {
    g_invalid_input = true;
    return 0;
  }
  if (1 == len) return nums[0];
  g_invalid_input = false;
  int r = nums[0];
  int count{1};
  for (int i = 1; i < len - 1; ++i) {
    if (0 == count) {
      r = nums[i];
      count = 1;
    } else if (nums[i] != r)
      --count;
    else
      ++count;
  }
  return r;
}

// 判断数字在数组中是否超过一半
bool is_repeat_more_than_half(int *nums, int len, int value) {
  int count{0};
  for (int i = 0; i < len; ++i) {
    if (value == nums[i]) ++count;
  }
  return count > (len >> 1);
}

// 打印数组中最小的k个数(二分法 快速排序的原理)
void find_lowest(int *nums, int len, int k) {
  if (!nums || len <= 0 || len < k) return;
  int index = partition(nums, 0, len - 1);
  while (index != k - 1) {
    if (index > k - 1)
      index = partition(nums, 0, index - 1);
    else
      index = partition(nums, index + 1, len - 1);
  }
  for (int i = 0; i < k; ++i)
    printf("%d\t", nums[i]);
  printf("\n");
}

// 查找数组子数组最大的和
int find_seq_sum_max(int *nums, int len) {
  if (!nums || len <= 0) {
    g_invalid_input = true;
    return 0;
  }
  g_invalid_input = false;
  int r{0};
  int cur_sum{0};
  for (int i = 0; i < len; ++i) {
    if (cur_sum < 0)
      cur_sum = nums[i];
    else
      cur_sum += nums[i];
    if (cur_sum > r) r = cur_sum;
  }
  return r;
}

// 一个数含1的数量(有更好的解法)
int num_of_1(int num) {
  int r{0};
  while (num) {
    if (1 == (num % 10))
      ++r;
    num = num / 10;
  }
  return r;
}

// 查找字符串中首个未重复的字符
char find_first_not_repeat_char(char *str) {
  int counts[256];
  for (int i = 0; i < 256; ++i)
    counts[i] = 0;
  char *p = str;
  while (*p) {
    ++counts[(int)*p++];
  }
  p = str;
  while (*p) {
    if (1 == counts[(int)*p])
      return *p;
    ++p;
  }
  return '\0';
}

int get_list_count(list_node_t *head) {
  if (!head) return 0;
  int r{1};
  while (head->next) {
    ++r;
    head = head->next;
  }
  return r;
}

list_node_t *find_first_common_node(list_node_t *head1, list_node_t *head2) {
  if (!head1 || !head2) return nullptr;
  if (head1 == head2) return head1;
  int count1 = get_list_count(head1);
  int count2 = get_list_count(head2);
  list_node_t *r{nullptr};
  int diff = count2 - count1;
  if (diff > 0) {
    while (head2->next && diff != 0) {
      head2 = head2->next;
      --diff;
    }
  } else if (diff < 0) {
    while (head1->next && diff != 0) {
      head1 = head1->next;
      ++diff;
    }
  }
  while (head1) {
    if (head1 == head2) {
      r = head1;
      break;
    }
    head1 = head1->next;
    head2 = head2->next;
  }
  return r;
}

bool is_balanced1_recursive(bin_node_t *root, int &depth) {
  if (!root) {
    depth = 0;
    return true;
  }
  int left{0};
  int right{0};
  if (is_balanced1_recursive(root, left) && 
      is_balanced1_recursive(root, right)) {
    int diff = left - right;
    if (diff <= 1 && diff >= -1) {
      depth = 1 + (diff > 0 ? left : right);
      return true;
    }
  }
  return false;
}

bool is_balanced1(bin_node_t *root) {
  if (!root) return false;
  int depth{0};
  return is_balanced1_recursive(root, depth);
}

void probability1(int origin, int index, int cur, int *times_list) {
  if (origin == 1) {
    times_list[cur - origin]++;
    return;
  }
  for (int i = 1; i <= g_rand_max_number; ++i)
    probability1(origin, index - 1, i + cur, times_list);
}

void probability1(int num, int *times_list) {
  for (int i = 1; i <= g_rand_max_number; ++i)
    probability1(num, num, i, times_list);
}

template <unsigned int N>
class CalcNum {
 public:
   enum {n = CalcNum<N - 1>::n + 1};
};

template <>
class CalcNum<1> {
 public:
   enum {n = 0};
};

int add_by_bit(int a, int b) {
  if (0 == b) return a;
  int sum = a ^ b;
  int carry = (a & b) >> 1;
  return add_by_bit(sum, carry);
}

/**
 * epoll中的LT(水平触发)和ET(边缘触发)模式区别
 * LT:
 * 1、读：当读缓冲区有数据的时候就会通知
 * 2、写：当写缓冲区不满的时候就会通知
 * ET:
 * 1、读：只有读缓冲区数据从空转为非空时才通知(通知一次)
 * 2、写：只有写缓冲区从满转为不满的时候才通知(通知一次)
 **/

/**
 * select 适合活跃比较多连接数目比较少的情况
 * epoll 适合连接数很多但大多数都不太活跃的情况
 * 
 * select 判断读写的时候需要对所有集合中的数据遍历 fd_set是位图存储方式
 * epoll 系统会拷贝有读写的那些连接
 *
 * epoll 提供了create ctl wait三个API，其中wait和select类似都可能阻塞IO
 *
 **/

/**
 * 阻塞IO: 等待数据准备好的时候阻塞
 * 非阻塞IO: 不用等待数据准备好直接返回
 * BIO/NBIO在数据准备好之后，由系统态拷贝到用户态时都会发生阻塞
 * fcntlarg_ex 在linux下可以设置是否为非阻塞IO
 *
 * 异步IO: 在等待数据准备前就直接返回，其拷贝的数据由系统进行通知(调用注册回调)
 * 
 * IO多路复用：为了解决一个线程只能进行一个连接的读写处理，可以让线程处理多个
 * socket连接
 *
 **/

/**
 * reactor 模式：NBIO + IO多路复用
 *
 **/

int str_to_num(char *str) {
  g_invalid_input = true;
  if (!str || '\0' == *str) {
    return 0;
  }
  auto digit = str;
  bool minus{false};
  if ('+' == *str){
    ++digit;
  } else if ('-' == *str) {
    ++digit;
    minus = true;
  }
  int r{0};
  while (*digit) {
    if ('0' < *digit && *digit < '9') {
      r = r * 10 + *digit - '0';
    } else {
      r = 0;
      break;
    }
    ++digit;
  }
  if ('\0' == *digit) {
    g_invalid_input = false;
    if (minus) r = -r;
  }
  return r;
}

int poke_compare1(const void *arg1, const void *arg2) {
  return *(int *)arg1 - *(int *)arg2;
}

bool is_poke_continuous1(int *nums, int len) {
  if (!nums || len < 2) return false;
  qsort(nums, len, sizeof(int), poke_compare1);
  int zero_num{0};
  for (int i = 0; i < len; ++i) {
    if (0 == nums[i])
      ++zero_num;
    else
      break;
  }
  int cap_num{0};
  for (int i = zero_num - 1; i < len - 1; ++i) {
    if (nums[i] == nums[i + 1]) return false; // 如果有相等的则不是顺子
    cap_num += nums[i + 1] - nums[i] - 1;
  }
  return cap_num > zero_num ? false : true;
}

void probability2(int origin, int index, int sum, int *times_list) {
  if (1 == index) {
    ++times_list[sum - origin];
  } else {
    for (int i = 1; i <= g_rand_max_number; ++i)
      probability2(origin, index - 1, i + sum, times_list);
  }
}

void probability2(int num, int *times_list) {
  for (int i = 1; i <= g_rand_max_number; ++i)
    probability2(num, num, i, times_list);
}

void reverse(char *begin, char *end) {
  if (!begin || !end) return;
  while (begin < end) {
    char temp = *begin;
    *begin++ = *end;
    *end-- = temp;
  }
}

void print_sum_seq1(unsigned int n, unsigned int sum) {
  unsigned int mid = (n + 1) >> 1;
  unsigned int start = 1;
  unsigned int end = start + 1;
  unsigned int cur_sum = start + end;
  while (start < mid) {
    if (cur_sum == sum) {
      for (unsigned int i = start; i <= end; ++i)
        printf("%d\t", i);
      printf("\n");
    }
    while (cur_sum > sum && start < mid) {
      cur_sum -= start;
      ++start;
      if (cur_sum == sum) {
        for (unsigned int i = start; i <= end; ++i)
          printf("%d\t", i);
        printf("\n");
      }
    }
    ++end;
    cur_sum += end;
  }
}

bool find_sum_num(int *nums, int len, int sum) {
  if (!nums || len <= 1) return false;
  int start = 0, end = len - 1;
  int cur_sum{0};
  bool r{false};
  while (start < end) {
    cur_sum = nums[start] + nums[end];
    if (cur_sum == sum) {
      printf("%d\t%d\n", nums[start], nums[end]);
      r = true;
      break;
    } else if (cur_sum > sum) {
      --end;
    } else {
      ++start;
    }
  }
  return r;
}

int find_first_bit1(int num) {
  int r{0};
  while ((num & 0x1) != 0x1 && r < (int)sizeof(num) * 8) {
    num = num >> 1;
    ++r;
  }
  return r;
}

bool is_bit1(int num, int index) {
  return 0x1 == (num & (0x1 << index));
}

void find_not_repeat_num(int *nums, int len) {
  if (!nums || len < 2) return;
  int xor_value{0};
  for (int i = 0; i < len; ++i)
    xor_value ^= nums[i];
  int num1{0}, num2{0};
  int first_bit1 = find_first_bit1(xor_value);
  for (int i = 0; i < len; ++i) {
    if (is_bit1(nums[i], first_bit1))
      num1 ^= nums[i];
    else
      num2 ^= nums[i];
  }
  printf("num1: %d, num2: %d", num1, num2);
}

bool is_banlanced1(bin_node_t *root, int &depth) {
  if (!root) {
    depth = 1;
    return true;
  }
  int left{0}, right{0};
  if (is_banlanced1(root, left) && is_banlanced1(root, right)) {
    int diff = left - right;
    if (diff <= 1 && diff >= -1) {
      depth = 1 + (left > right ? left : right);
      return true;
    }
  }
  return false;
}

int deep_tree(bin_node_t *root) {
  if (!root) return 0;
  int left{0}, right{0};
  if (root->left) left = deep_tree(root->left);
  if (root->right) right = deep_tree(root->right);
  return 1 + (left > right ? left : right);
}

int find_first_num_of_k(int *nums, int len, int k) {
  int r{-1};
  int mid = (len - 1) >> 1;
  int start{0};
  int end{len - 1};
  while (start < end) {
    if (nums[mid] == k) {
      if (0 == mid || nums[mid - 1] != k) {
        r = mid;
        break;
      } else {
        start = mid - 1;
      }
    } else if (nums[mid] < k) {
      start = mid;
    } else {
      end = mid;
    }
    mid = (end + start) >> 1;
  }
  return r;
}

int find_last_num_of_k(int *nums, int len, int k) {
  int mid = (len - 1) >> 1;
  int r{-1};
  int start{0};
  int end{len - 1};
  while (start < end) {
    if (nums[mid] == k) {
      if ((mid == len - 1 || nums[mid + 1] != k)) {
        r = mid;
        break;
      } else {
        end = mid + 1;
      }
    } else if (nums[mid] > k) {
      end = mid;
    } else {
      start = mid;
    }
    mid = (start + end) >> 1;
  }
  return r;
}

int count1(list_node_t *head) {
  if (!head) return 0;
  int r{0};
  while (head) {
    head = head->next;
    ++r;
  }
  return r;
}

void convert_to_node_recursive(bin_node_t *root, bin_node_t **node) {
  if (!root) {
    return;
  }
  if (root->left) convert_to_node_recursive(root->left, node);
  root->left = *node;
  if (*node) (*node)->right = root;
  *node = root;
  if (root->right) convert_to_node_recursive(root->right, node);
}

bin_node_t *convert_to_node(bin_node_t *root) {
  if (!root) return nullptr;
  bin_node_t *node{nullptr};
  convert_to_node_recursive(root, &node);
  while (node->left)
    node = node->left;
  return node;
}

// 判断一个数字是否在指定的矩阵中
// 该矩阵从左到右递增 从上到下递增
bool is_find_in_matrix(int **nums, int row, int column, int value) {
  if (!nums || row <= 0 || column <= 0) return false;
  int i{0}, j{column - 1};
  bool r{false};
  while (i < row && j > 0) {
    if (nums[i][j] == value) {
      r = true;
      break;
    } else if (nums[i][j] > value) {
      --j;
    } else {
      ++i;
    }
  }
  return r;
}

// 线程安全的单例类
class Singleton {

 public:
   static std::shared_ptr<Singleton> get_instance();
 
 private:
   Singleton() {}
   ~Singleton() {}
   Singleton(const Singleton &);
   Singleton &operator=(const Singleton &);

};

static std::shared_ptr<Singleton> g_singleton{nullptr};
std::once_flag g_singleton_once_flag;
std::shared_ptr<Singleton> Singleton::get_instance() {
  std::call_once(g_singleton_once_flag, [&] {
    g_singleton = std::shared_ptr<Singleton>(new Singleton);
  });
  return g_singleton;
};

class MyStr {

 public:
   MyStr(char *d = nullptr) {
     data_ = nullptr;
     if (d) {
       int len = strlen(d);
       data_ = new char[len + 1];
       strcpy(data_, d);
       data_[len + 1] = '\0';
     }
   }

   MyStr(const MyStr &obj) {
    data_ = nullptr;
     if (obj.data_) {
       int len = strlen(obj.data_);
       data_ = new char[len + 1];
       strcpy(data_, obj.data_);
       data_[len + 1] = '\0';
     }
   }
   
   MyStr &operator=(const MyStr &obj) {
     if (this == &obj) return *this;
     MyStr temp(obj);
     auto data_temp{temp.data_};
     temp.data_ = data_;
     data_ = data_temp;
   }

 private:
   char *data_;

};

// 求一个数（由低位到高位）前一位能被1整除，前两位能被2整除，前N位能被N整除
bool used[9];
void find_nums(int n, int m, std::vector<int> &nums) {
  if (m != 0 && (n % m) != 0) return;
  if (9 == m) {
    nums.push_back(n);
  } else {
    for (int i = 1; i <= 9; ++i) {
      if (!used[i]) {
        used[i] = true;
        find_nums(n * 10 + i, m + 1, nums);
        used[i] = false;
      }
    }
  }
}

// 获得数组中最小的n个数(利用std::set 达到nlogn的查找速度)
void find_least_list(
    const std::vector<int> &list, std::set< int, std::greater<int> > &r, int n) {
  for (auto v : list) {
    if (r.size() < static_cast<size_t>(n)) {
      r.insert(v);
    } else {
      if (v < *r.begin()) {
        r.erase(r.begin());
        r.insert(v);
      }
    }
  }
}

// 一个数组中只有两个数字没有重复 其他都重复了，求这两个数
void find_repeat_once_num(int *nums, int len, int &num1, int num2) {
  if (!nums || len <= 0) return;
  int num{0};
  for (int i = 0; i < len; ++i)
    num ^= nums[i];
  if (0 == num) return;
  int first_bit1 = get_first_bit_1(num);
  num1 = num2 = 0;
  for (int i = 0; i < len; ++i) {
    if (is_bit1(nums[i], first_bit1))
      num1 ^= nums[i];
    else
      num2 ^= nums[i];
  }
}

// 获得数组中和为指定值的数字对
void find_pairs_sum(int *nums, int len, int sum) {
  if (!nums || len <= 0) return;
  int i{0}, j{len - 1};
  int cur_sum{0};
  while (i < j) {
    cur_sum = nums[i] + nums[j];
    if (cur_sum == sum) {
      printf("%d\t%d", nums[i], nums[j]);
    } else if (cur_sum > sum) {
      --j;
    } else {
      ++i;
    }
  }
}

// 圆圈中最后一个数字的公式解法
int get_last_num(int n, int m) {
  if (n < 1 || m < 1) return -1;
  int r{0};
  for (int i = 2; i <= n; ++i)
    r = (r + m) % i;
  return r;
}

// 将数字转换为字符串
const char *convert(char buf[], int value) {
  if (!buf || 0 == value) return buf;
  char digits[] = {'9', '8', '7', '6', '5', '4', '3', '2', '1',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  char *zero = digits + 9;
  int temp{value};
  char *p{buf};
  while (temp) {
    int index = temp % 10;
    *p++ = zero[index];
    temp /= 10;
  }
  *p = '\0';
  if (value < 0) *p++ = '-';
  std::reverse(buf, p);
  return buf;
}
