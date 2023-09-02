#ifndef __DIJKSTRA_H_
#define __DIJKSTRA_H_

#include <vector>
#include <unordered_map>
#include <list>
#include <tuple>
#include <queue>
#include <optional>
#include "fheap.hpp"

// A vertex is typed as `vertex_t`. An edge is typed as `edge_t`.
using vertex_t = std::size_t;
using edge_weight_t = float;
using edge_t = std::tuple<vertex_t, vertex_t, edge_weight_t>;
using edges_t = std::vector<edge_t>;

enum class GraphType {
UNDIRECTED,
DIRECTED
};


class Graph {
    public:
        Graph() = delete;
        Graph(const Graph&) = delete;
        Graph(Graph&&) = delete;

        // We assume that if num_vertices is V, a graph contains n vertices from 0 to V-1.
        Graph(size_t num_vertices, const edges_t& edges, GraphType type)
			: num_vertices(num_vertices), type(type) {
			this->num_vertices = num_vertices;
			graph.resize(num_vertices);

			if(type == GraphType::UNDIRECTED) {
				for(auto &edge : edges) {
					const auto& [ from, to, weight ] = edge;
					graph[from].emplace_back(from, to, weight);
					graph[to].emplace_back(to, from, weight);
				}
			} else {
				for(auto &edge : edges) {
					const auto& [ from, to, weight ] = edge;
					graph[from].emplace_back(from, to, weight);
				}
			}
        }

		size_t get_num_vertices() { return num_vertices; }
		std::vector<edge_t> adj_list(vertex_t v) { return graph[v]; }
		
    private:
		size_t num_vertices;
		std::vector<std::vector<edge_t>> graph;
		GraphType type;
};


std::unordered_map<vertex_t, std::optional<std::tuple<vertex_t, edge_weight_t>>>
dijkstra_shortest_path(Graph& g, vertex_t src) {
	
	std::unordered_map<vertex_t, std::optional<std::tuple<vertex_t, edge_weight_t>>> M;

    // std::nullopt if vertex v is not reacheble from the source.
    for(vertex_t v = 0; v < g.get_num_vertices(); v++) M[v] = std::nullopt;
	std::vector<edge_weight_t> dist(g.get_num_vertices(), 1e10);

    // TODO
	dist[src] = (edge_weight_t)(0);
	FibonacciHeap<std::tuple<edge_weight_t, vertex_t>> heap = {};
	std::vector<std::shared_ptr<FibonacciNode<std::tuple<edge_weight_t, vertex_t>>>> nodes(g.get_num_vertices(),nullptr);
	for(vertex_t v=0; v<g.get_num_vertices(); v++)
	{
		nodes[v] = std::make_shared<FibonacciNode<std::tuple<edge_weight_t, vertex_t>>>(std::make_tuple(dist[v], v));
		heap.insert(nodes[v]);
	}

	vertex_t* prev = new vertex_t[(int)g.get_num_vertices()];
	prev[src] = src;

	while(heap.size())
	{
		vertex_t u = std::get<1>(heap.extract_min().value());
		if(dist[u]==1e10) break;//INF, unreachable
		M[u] = std::make_tuple(prev[u], dist[u]);

		std::vector<edge_t> adj = g.adj_list(u);
		
		for(vertex_t i=0;i<adj.size();i++)
		{
			vertex_t v = std::get<1>(adj[i]);
			edge_weight_t w = std::get<2>(adj[i]);
			if(w+dist[u] < dist[v])
			{
				dist[v] = w+dist[u];
				prev[v] = u;
				heap.decrease_key(nodes[v], std::make_tuple(w+dist[u], v));
			}
		}
	}

	//M: index 0 is prev, index 1 is dist
	delete[] prev;
	return M;
}

#endif // __DIJKSTRA_H_

//valgrind --leak-check=yes ./test
//valgrind --leak-check=yes --leak-check=full --show-leak-kinds=all ./test