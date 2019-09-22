#include "UnixFileSystem.hpp"
#include "Node.hpp"
#include "Tree.hpp"
#include <queue>
#include <iostream> // for debug
#include <string>
#include "dirent.h"
#include <cerrno>

using namespace dv;

UnixFileSystem::UnixFileSystem()
{

}

UnixFileSystem::~UnixFileSystem()
{

}

//handle ERRNO properly, consider passing int codes above to signal >error
int UnixFileSystem::GetDataStartingFrom(const std::string& prefix, const std::string& rootName, std::unique_ptr<Tree>& outTree, unsigned int callerIndex)
{
    std::string path = prefix + rootName; //TODO: revise if this can be simplified to avoid costly operations on strings 
    const std::string rootPath = path;
    Node* root = new Node(rootName, GenerateID(), EConnectionType::Normal, EFileType::Directory);
    std::unique_ptr<Tree> createdTree = std::make_unique<Tree>(root);
    std::queue<std::pair<Node*, const std::string>> directoryQueue;
    DIR* directory = opendir(rootPath.c_str());
    if(!directory)
        return errno;
    // handle root directory
    while(struct dirent* dirEntry = readdir(directory))
    {
        if(strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0)
            continue;
        std::cout << "Read Directory with name: " << dirEntry->d_name << std::endl;
        Node* newNode = new Node(dirEntry->d_name, GenerateID(), EConnectionType::Normal, ConvertFileType(dirEntry->d_type));// add size
        createdTree->AddNode(newNode, root);
        if(dirEntry->d_type == DT_DIR)
        {
            path = (rootPath + '/' + dirEntry->d_name);
            std::cout << "Added directory with path: " << path <<std::endl;
            directoryQueue.push({newNode, path});
        }
    }

    if(closedir(directory) == -1)
    {
        std::cout << "CHECK opening for root: " << directory << std::endl;
        return errno;
    }

    while(!directoryQueue.empty())
    {
        // DECISION
        // create Nodes on the fly and fill tree accordingly, this tree is later moved out to OUT parameter hence refilling currently existing tree
        // if boolean specifying we want only part of the tree is set, we need to unparse the tree outside (or fill it in some nice way on the fly)

        // add all children by exploring this directory and calling a while loop with readdir, for each child which is itself a directory add it to a tree
        // remember about adding nodes to queue and children both, moreover maybe remember about level order? so that we can easily remap it into tree structure
        // this way we introduce 1-1 mapping between data tree(which is assisted with controller itself -locations) and data from FS (maybe it should be fused into one?)

        auto currentDirectory = directoryQueue.front();
        Node* parent = currentDirectory.first;
        directoryQueue.pop();
        // open currently picked up node
        directory = opendir(currentDirectory.second.c_str());
        if(!directory)
        {
            std::cout << "CHECK opening for directory: " << currentDirectory.second<< std::endl;
            return errno;
        }
        while(struct dirent* dirEntry = readdir(directory)) //refactor this to separate private function 
        {
            if(strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0)
                continue;
            std::cout << "Read Directory with name: " << dirEntry->d_name << std::endl;
            Node* newNode = new Node(dirEntry->d_name, GenerateID(), EConnectionType::Normal, ConvertFileType(dirEntry->d_type));// add size
            createdTree->AddNode(newNode, parent);
            if(dirEntry->d_type == DT_DIR)
            {
                path = (currentDirectory.second + '/' + dirEntry->d_name);
                directoryQueue.push({newNode, path});
            }
        }
        if(closedir(directory) == -1)
        {
            std::cout << "CHECK closing `for directory: " << directory << std::endl;
            return errno;
        }
    }
     
    
    return 0; //return status OK
}

bool UnixFileSystem::Write(unsigned int callerIndex)
{

    return true;
}

bool UnixFileSystem::RefreshState()
{

    return true;
}
bool UnixFileSystem::ObtainAdditionalData()
{
    return true;
}

EFileType UnixFileSystem::ConvertFileType(unsigned char fType) // TODO: expand it
{
    switch(fType)
    {
        case DT_DIR:
            return EFileType::Directory;
        case DT_REG:
            return EFileType::File;
        case DT_LNK:
            return EFileType::Symlink;
        case DT_UNKNOWN:
            return EFileType::Invalid;
        default:
            return EFileType::Other;
    }
}