//
//  mqtt_subscription_tree.h
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

#ifndef acatl_mqtt_subscription_tree_h
#define acatl_mqtt_subscription_tree_h

#include <acatl_mqtt/mqtt_session.h>
#include <acatl_mqtt/mqtt_topic.h>

#include <unordered_map>
#include <set>


namespace acatl
{
    namespace mqtt
    {

        typedef std::set<Session::Ptr> Sessions;
        
        class SubscriptionNodeBase
        {
        public:
            virtual ~SubscriptionNodeBase() {}
            
            typedef std::unique_ptr<SubscriptionNodeBase> Ptr;
            
            bool match(TopicHierarchyIterator cur, const TopicHierarchyIterator& end, Sessions& sessions, std::error_code& ec) const
            {
                bool result = true;
                if(cur != end) {
                    result = doMatch(cur, end, sessions, ec);
                } else {
                    sessions.insert(std::begin(_sessions), std::end(_sessions));
                }
                return result;
            }
            
            bool addFilter(TopicHierarchyIterator cur, const TopicHierarchyIterator& end, Session::Ptr session, std::error_code& ec)
            {
                bool result = true;
                if(cur != end) {
                    result = doAddFilter(cur, end, session, ec);
                } else {
                    _sessions.emplace(session);
                }
                return result;
            }
            
            virtual void dump(std::ostream& stream, size_t indent) const = 0;
            
            virtual SubscriptionNodeBase::Ptr clone() const = 0;
            
        protected:
            SubscriptionNodeBase() = default;
            Sessions _sessions;
            
        private:
            virtual bool doMatch(TopicHierarchyIterator cur, const TopicHierarchyIterator& end, Sessions& sessions, std::error_code& ec) const = 0;
            virtual bool doAddFilter(TopicHierarchyIterator cur, const TopicHierarchyIterator& end, Session::Ptr session, std::error_code& ec) = 0;
        };
        
        
        template<typename T>
        struct NodeCreator
        {
            static SubscriptionNodeBase::Ptr createTopicNode(const TopicHierarchyIterator& cur);
            static SubscriptionNodeBase::Ptr createMulitLevelWildCardNode(const TopicHierarchyIterator& cur);
            static SubscriptionNodeBase::Ptr createSingleLevelWildCardNode(const TopicHierarchyIterator& cur);
        };

        
        class IntermediateSubscriptionNode : public SubscriptionNodeBase
        {
        protected:
            IntermediateSubscriptionNode() = default;
            
            std::unordered_map<std::string, SubscriptionNodeBase::Ptr> _nodes;
            
        private:
            bool doMatch(TopicHierarchyIterator cur, const TopicHierarchyIterator& end, Sessions& sessions, std::error_code& ec) const override
            {
                bool result = false;
                
                auto it = _nodes.find(*cur);
                ++cur;
                if(it != _nodes.end()) {
                    result |= it->second->match(cur, end, sessions, ec);
                }
                it = _nodes.find("#");
                if(it != _nodes.end()) {
                    result |= it->second->match(cur, end, sessions, ec);
                }
                it = _nodes.find("+");
                if(it != _nodes.end()) {
                    result |= it->second->match(cur, end, sessions, ec);
                }
                
                return result;
            }

            bool doAddFilter(TopicHierarchyIterator cur, const TopicHierarchyIterator& end, Session::Ptr session, std::error_code& ec) override
            {
                bool ret = true;
                
                if(*cur == "#") {
                    auto result = _nodes.emplace(*cur, NodeCreator<SubscriptionNodeBase>::createMulitLevelWildCardNode(cur));
                    ret = result.first->second->addFilter(++cur, end, session, ec);
                } else if(*cur == "+") {
                    auto result = _nodes.emplace(*cur, NodeCreator<SubscriptionNodeBase>::createSingleLevelWildCardNode(cur));
                    ret = result.first->second->addFilter(++cur, end, session, ec);
                } else {
                    auto result = _nodes.emplace(*cur, NodeCreator<SubscriptionNodeBase>::createTopicNode(cur));
                    ret = result.first->second->addFilter(++cur, end, session, ec);
                }
                
                return ret;
            }
        };
        
        
        class RootSubscriptionNode : public IntermediateSubscriptionNode
        {
        public:
            SubscriptionNodeBase::Ptr clone() const override
            {
                std::unique_ptr<RootSubscriptionNode> root(new RootSubscriptionNode);
                
                for(const auto& node : _nodes) {
                    root->_nodes.emplace(node.first, node.second->clone());
                }
                
                return std::move(root);
            }
            
        private:
            void dump(std::ostream& stream, size_t indent) const override
            {
                for(const auto& node : _nodes) {
                    node.second->dump(stream, indent);
                }
            }
        };
        
        
        class TopicSubscriptionNode : public IntermediateSubscriptionNode
        {
        public:
            TopicSubscriptionNode(const std::string& topic)
            : _topic(topic)
            {}
            
            SubscriptionNodeBase::Ptr clone() const override
            {
                std::unique_ptr<TopicSubscriptionNode> topicNode(new TopicSubscriptionNode(_topic));
                
                for(const auto& node : _nodes) {
                    topicNode->_nodes.emplace(node.first, node.second->clone());
                }
                for(const auto& session : _sessions) {
                    topicNode->_sessions.emplace(session);
                }
                
                return std::move(topicNode);
            }

        private:
            void dump(std::ostream& stream, size_t indent) const override
            {
                stream << std::string(indent, ' ') << std::quoted(_topic);
                if(!_sessions.empty()) {
                    stream << " -> ";
                    for(const auto& session : _sessions) {
                        stream << session->clientId() << ",";
                    }
                }
                stream << "\n";
                indent += 2;
                for(const auto& node : _nodes) {
                    node.second->dump(stream, indent);
                }
            }
            
            std::string _topic;
        };
        
        
        class MultiLevelWildCardSubscriptionNode : public SubscriptionNodeBase
        {
        public:
            SubscriptionNodeBase::Ptr clone() const override
            {
                std::unique_ptr<MultiLevelWildCardSubscriptionNode> multiNode(new MultiLevelWildCardSubscriptionNode);
                for(const auto& session : _sessions) {
                    multiNode->_sessions.emplace(session);
                }

                return std::move(multiNode);
            }

        private:
            bool doMatch(TopicHierarchyIterator cur, const TopicHierarchyIterator& end, Sessions& sessions, std::error_code& ec) const override
            {
                sessions.insert(std::begin(_sessions), std::end(_sessions));
                return true;
            }
            
            bool doAddFilter(TopicHierarchyIterator cur, const TopicHierarchyIterator& end, Session::Ptr session, std::error_code& ec) override
            {
                ec = mqtt_error::invalid_topic_filter;
                return false;
            }
            
            void dump(std::ostream& stream, size_t indent) const override
            {
                stream << std::string(indent, ' ') << std::quoted("#");
                if(!_sessions.empty()) {
                    stream << " -> ";
                    for(const auto& session : _sessions) {
                        stream << session->clientId() << ",";
                    }
                }
                stream << "\n";
            }
        };
        

        class SingleLevelWildCardSubscriptionNode : public IntermediateSubscriptionNode
        {
        public:
            SubscriptionNodeBase::Ptr clone() const override
            {
                std::unique_ptr<SingleLevelWildCardSubscriptionNode> singleNode(new SingleLevelWildCardSubscriptionNode);
                
                for(const auto& node : _nodes) {
                    singleNode->_nodes.emplace(node.first, node.second->clone());
                }
                for(const auto& session : _sessions) {
                    singleNode->_sessions.emplace(session);
                }

                return std::move(singleNode);
            }

        private:
            void dump(std::ostream& stream, size_t indent) const override
            {
                stream << std::string(indent, ' ') << std::quoted("+");
                if(!_sessions.empty()) {
                    stream << " -> ";
                    for(const auto& session : _sessions) {
                        stream << session->clientId() << ",";
                    }
                }
                stream << "\n";
                indent += 2;
                for(const auto& node : _nodes) {
                    node.second->dump(stream, indent);
                }
            }
        };
        

        template<typename T>
        SubscriptionNodeBase::Ptr NodeCreator<T>::createTopicNode(const TopicHierarchyIterator& cur)
        {
            return SubscriptionNodeBase::Ptr(new TopicSubscriptionNode(*cur));
        }
        
        template<typename T>
        SubscriptionNodeBase::Ptr NodeCreator<T>::createMulitLevelWildCardNode(const TopicHierarchyIterator& cur)
        {
            return SubscriptionNodeBase::Ptr(new MultiLevelWildCardSubscriptionNode());
        }
        
        template<typename T>
        SubscriptionNodeBase::Ptr NodeCreator<T>::createSingleLevelWildCardNode(const TopicHierarchyIterator& cur)
        {
            return SubscriptionNodeBase::Ptr(new SingleLevelWildCardSubscriptionNode());
        }
        
        
        class SubscriptionTree
        {
        public:
            typedef std::shared_ptr<SubscriptionTree> Ptr;
            typedef std::shared_ptr<const SubscriptionTree> ConstPtr;
            
            SubscriptionTree()
            : _rootNode(new RootSubscriptionNode())
            {}
            
            bool match(const TopicName& topic, Sessions& sessions, std::error_code& ec) const
            {
                return _rootNode->match(topic.begin(), topic.end(), sessions, ec);
            }
            
            bool addFilter(const TopicFilter& filter, Session::Ptr session, std::error_code& ec)
            {
                return _rootNode->addFilter(filter.begin(), filter.end(), session, ec);
            }
            
            void dump(std::ostream& stream, size_t indent) const
            {
                _rootNode->dump(stream, indent);
            }
            
        private:
            friend class SubscriptionTreeManager;
            
            SubscriptionTree(SubscriptionNodeBase::Ptr rootNode)
            : _rootNode(std::move(rootNode))
            {}
            
            SubscriptionTree::Ptr clone() const
            {
                return SubscriptionTree::Ptr(new SubscriptionTree(_rootNode->clone()));
            }

            SubscriptionNodeBase::Ptr _rootNode;
        };

    }
}

#endif 
