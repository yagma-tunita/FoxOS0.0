#pragma once

#include "types.h"
#include "utils.h"

namespace list
{
    struct Node
    {
        void* data;
        Node* next;
    };

    struct List
    {
        Node* head;
        Node* tail;
        size_t length;
    };

    // 初始化链表
    inline void init(List* list)
    {
        if (!list) return;
        list->head = nullptr;
        list->tail = nullptr;
        list->length = 0;
    }

    // 判断链表是否为空
    inline bool empty(const List* list)
    {
        return (list && list->head == nullptr) || !list;
    }

    // 清空链表（仅释放节点，不释放数据）
    inline void clear(List* list)
    {
        if (!list) return;
        Node* current = list->head;
        while (current)
        {
            Node* next = current->next;
            delete current;
            current = next;
        }
        list->head = nullptr;
        list->tail = nullptr;
        list->length = 0;
    }

    // 尾插法添加节点
    inline void push_back(List* list, void* data)
    {
        if (!list || !data) return;
        Node* new_node = new Node;
        new_node->data = data;
        new_node->next = nullptr;

        if (empty(list))
        {
            list->head = new_node;
            list->tail = new_node;
        }
        else
        {
            list->tail->next = new_node;
            list->tail = new_node;
        }
        list->length++;
    }

    // 遍历链表（回调函数处理每个节点数据）
    inline void foreach(const List* list, void (*callback)(void*))
    {
        if (!list || !callback) return;
        Node* current = list->head;
        while (current)
        {
            callback(current->data);
            current = current->next;
        }
    }
}