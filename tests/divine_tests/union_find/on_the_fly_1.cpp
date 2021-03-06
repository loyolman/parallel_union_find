#include <thread>
#include <vector>
#include <cassert>

#include "union_find/blocking/on_the_fly_scc_union_find.hpp"
#include "graph_node/on_the_fly_scc_graph_node.hpp"
#include "algorithm/multi_core_on_the_fly_scc_decomposition_algorithm.hpp"

using namespace std;
using namespace parallel_union_find::union_find::blocking;
using namespace parallel_union_find::graph_node;
using namespace parallel_union_find::algorithm;

using node = on_the_fly_scc_graph_node<on_the_fly_scc_union_find>;

vector<node> nodes(4);

int main()
{
    // set edges 0 -> 1, 1 -> 2, 2 -> 3, 3 -> 3, 3 -> 0
    nodes.at(0).add_son(&nodes.at(1));
    nodes.at(1).add_son(&nodes.at(2));
    nodes.at(2).add_son(&nodes.at(3));
    nodes.at(3).add_son(&nodes.at(3));
    nodes.at(3).add_son(&nodes.at(0));

    std::thread t1([](){
        while (!nodes.at(0).union_set(&nodes.at(1)));

        assert(nodes.at(0).same_set(&nodes.at(1)));
        nodes.at(1).add_mask(1);

        for (int i = 0; i < 5; ++i)
            if (i == 3)
                nodes.at(1).mark_as_done();
            else
                nodes.at(0).get_node_from_set();

        assert(nodes.at(1).is_done());
    });
    std::thread t2([](){
        while (!nodes.at(0).union_set(&nodes.at(2)));

        assert(nodes.at(0).same_set(&nodes.at(2)));
        nodes.at(2).add_mask(2);

        for (int i = 0; i < 5; ++i)
            if (i == 3)
                nodes.at(2).mark_as_done();
            else
                nodes.at(0).get_node_from_set();

        assert(nodes.at(2).is_done());
    });

    t1.join();
    t2.join();
}

