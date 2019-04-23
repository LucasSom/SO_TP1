#!/usr/bin/python3
# -*- coding: utf-8 -*-

import networkx as nx
import random

# grafo de n nodos donde la probabilidad de que un eje exista es de p
n = 300
p = 0.4
G = nx.erdos_renyi_graph(n, p)

# Esto es re sucio, y puede tardar sobretodo con un p chico
# Pero es bien random :)
while(not nx.is_connected(G)):
    G = nx.erdos_renyi_graph(n, p)
for (u, v, w) in G.edges(data=True):
    w['weight'] = random.randint(0, 10)

m = G.number_of_edges()
tamanios = "{}\n{}\n".format(n, m)
tamaniosBytes = bytes(tamanios, "utf-8")
with open("output.txt", "wb") as file:  # te hace el open y el close
    file.write(tamaniosBytes)
    nx.write_weighted_edgelist(G, file)
