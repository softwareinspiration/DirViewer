#pragma once

#include <Defines.hpp>
#include "Util.hpp"

namespace dv
{
    class Node;

    struct TreeSpan
    {
        TreeSpan(const TreeSpan&& other) //for (N)RVO
        {
            Nodes = std::move(other.Nodes);
            LevelNodeCount = std::move(other.LevelNodeCount);
        }

        TreeSpan(){};

        std::vector<Node*> Nodes;
        std::vector<size_t> LevelNodeCount; //holds count of total node at this level (counting from root)
    };

    class Tree
    {
        public:
            Tree(Node* root);
            ~Tree();
            void AddNode(Node* node, Node* parent); //for adding new nodes when parent node pointer is known
            bool RemoveNode(Node* node); //add removing discriminator
            TreeSpan GetTreeLevelOrder() const;
            std::stack<Node*> GetPathToRootFrom(Node* from) const;
        private:
            Node* FindNode(Node* toFind) const;
            Node* Root;
            Node* Dummy; //special kind of node to mark invalid
    };
}
