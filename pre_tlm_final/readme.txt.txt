TODO:
Types.hpp	which Types do we really need?
MemCtrl.cpp & Mlp.cpp which module waits for event e_mlp_start and which one notifies?
MemCtrl.cpp		line 40 - do we need y_deq? can't we use just data whitout an additional deque.
Checker.cpp		line 25 - when wait(1, SC_NS); is uncommented, vp does not print out results properly. add an event?
what data do we need beforehand? number of layers, number of inputs & max number of neurons per layer.