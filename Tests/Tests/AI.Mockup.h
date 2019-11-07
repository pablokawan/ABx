#pragma once

#include "Action.h"
#include "Registry.h"
#include "Filter.h"

namespace AI {

class TestAction : public Action
{
protected:
    Status DoAction(Agent& agent, uint32_t timeElapsed) override;
public:
    NODE_FACTORY(TestAction)
    explicit TestAction(const ArgumentsType& arguments) :
        Action(arguments)
    { }
};

class SelectSelf : public Filter
{
public:
    FILTER_FACTORY(SelectSelf)
    explicit SelectSelf(const ArgumentsType& arguments) :
        Filter(arguments)
    { }
    void Execute(Agent& agent) override;
};

class SelectNothing : public Filter
{
public:
    FILTER_FACTORY(SelectNothing)
    explicit SelectNothing(const ArgumentsType& arguments) :
        Filter(arguments)
    { }
    void Execute(Agent& agent) override;
};

class TestRegistry : public Registry
{
public:
    TestRegistry() :
        Registry()
    { }
    void Initialize() override;
};

}
