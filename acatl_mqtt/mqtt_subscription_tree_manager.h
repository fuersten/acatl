//
//  mqtt_subscription_tree_manager.h
//  acatl_mqtt
//
//  BSD 3-Clause License
//  Copyright (c) 2019, Lars-Christian Fürstenberg
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted
//  provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//  conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of
//  conditions and the following disclaimer in the documentation and/or other materials provided
//  with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors may be used to
//  endorse or promote products derived from this software without specific prior written
//  permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
//  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//

#ifndef acatl_mqtt_subscription_tree_manager_h
#define acatl_mqtt_subscription_tree_manager_h

#include <acatl_mqtt/mqtt_subscription_tree.h>


namespace acatl
{
    namespace mqtt
    {

        class SubscriptionTreeManager
        {
        public:
            class WritableTree
            {
            public:
                ~WritableTree()
                {
                    _manager.setTree(_tree);
                }
                
                SubscriptionTree* tree()
                {
                    return _tree.get();
                }
                
            private:
                friend class SubscriptionTreeManager;
                
                WritableTree(SubscriptionTree::Ptr tree, SubscriptionTreeManager& manager)
                : _manager(manager)
                , _tree(tree)
                {}
                
                SubscriptionTreeManager& _manager;
                SubscriptionTree::Ptr _tree;
            };

            
            SubscriptionTreeManager()
            : _tree(new SubscriptionTree)
            {
            }
            
            WritableTree getWritableTree()
            {
                _writeMutex.lock();
                return WritableTree(_tree->clone(), *this);
            }

            SubscriptionTree::ConstPtr getCurrentSubscriptionTree() const
            {
                std::unique_lock<std::mutex> guard(_readMutex);
                return _tree;
            }
            
        private:
            friend class WritableTree;
            
            bool setTree(SubscriptionTree::Ptr tree)
            {
                if(_writeMutex.try_lock()) {
                    return false;
                }
                std::unique_lock<std::mutex> guard(_readMutex);
                _tree = tree;
                _writeMutex.unlock();
                
                return true;
            }
            
            std::mutex _writeMutex;
            mutable std::mutex _readMutex;
            SubscriptionTree::Ptr _tree;
        };

    }
}

#endif 
