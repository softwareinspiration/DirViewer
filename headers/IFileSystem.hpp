#pragma once

#include <string>
#include <memory>

namespace dv
{
    class Tree;
    enum class EFileType : char;
    class IFileSystem
    {
        public:
        //TODO: document API
        virtual int GetDataStartingFrom(const std::string& srcPath, std::unique_ptr<Tree>& outTree, unsigned int callerIndex) = 0;

    //    virtual int Move(const std::string& srcPath, const std::string& dstPath) = 0;

        virtual int MakeFile(const std::string& path, EFileType fType) = 0;

     //   virtual int Remove(const std::string& prefix, const std::string& name) = 0;

        //Write current changes in filesystem tree hierarchy 
        virtual bool Write(unsigned int callerIndex) = 0;

        // Refresh current system state (maybe something has changed outside)
        virtual bool RefreshState() = 0;

        //Obtain additional data for current node? (modes etc?)
        virtual bool ObtainAdditionalData() = 0;

    };
};
