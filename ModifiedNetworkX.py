import networkx as nx
from heapq import heappush, heappop
from Classes import Path, Order, Orderbook
from itertools import count

def calcSlippageLoss(liquidity, q1):
    R1 = liquidity/2
    R2 = liquidity/2 #can remove for optimization later
    if (q1 >= R1):
        #insufficient liquidity
        return float('inf')
    #q2_prime = (R2*q1)/R1
    q2 = (R2*q1)/(R1+q1)
    return q1 - q2

def _slippage_dijkstra_alg(
    G, exchange_amount, source, target, paths = None, cutoff=None):
    G_succ = G._adj

    push = heappush
    pop = heappop
    dist = {}  # dictionary of final distances
    seen = {}
    # fringe is heapq with 3-tuples (distance,c,node)
    # use the count c to avoid comparing nodes (may not be able to)
    c = count()
    fringe = []
    if source not in G:
        raise nx.NodeNotFound(f"Source {source} not in G")
    seen[source] = 0
    push(fringe, (0, next(c), source))
    while fringe:
        (d, _, v) = pop(fringe)
        if v in dist:
            continue  # already searched this node.
        dist[v] = d
        if v == target:
            break
        for u, e in G_succ[v].items():
            cost = calcSlippageLoss(e['liquidity'], exchange_amount - dist[v])
            #cost = weight(v, u, e)
            if cost is None:
                continue
            vu_dist = dist[v] + cost
            if cutoff is not None:
                if vu_dist > cutoff:
                    continue
            if u in dist:
                u_dist = dist[u]
                if vu_dist < u_dist:
                    raise ValueError("Contradictory paths found:", "negative weights?")
            elif u not in seen or vu_dist < seen[u]:
                seen[u] = vu_dist
                push(fringe, (vu_dist, next(c), u))
                if paths is not None:
                    paths[u] = paths[v] + [u]

    # The optional predecessor and path dictionaries can be accessed
    return dist

def slippage_dijkstra(G, exchange_amount, source, target, cutoff=None):
    if target == source:
        return (0, [target])
    
    paths = {source: [source]} # dictionary of paths
    dist = _slippage_dijkstra_alg(
        G, exchange_amount, source, target, paths=paths, cutoff=cutoff, 
    )
    if target is None:
        return (dist, paths)
    try:
        return (dist[target], paths[target])
    except KeyError as e:
        raise nx.NetworkXNoPath(f"No path to {target}.") from e
