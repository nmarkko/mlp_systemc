#include "VP.hpp"
#include <iostream>


VP::VP(sc_module_name name) :
	sc_module(name),
	ic("Interconnect"),
	mlp("MultyLayerPerceptron"),
   prep("Preprocessor"),
   ram("RAM"),

   dma("DMA"),
   fifo(785)
{

	s_vp_t0.register_b_transport(this, &VP::b_transport0);
	s_vp_t1.register_b_transport(this, &VP::b_transport1);

   cout<<name<<" constructed"<<endl;
	s_vp_i0.bind(ic.s_ic_t);
   ic.s_ic_mlp.bind(mlp.s_mlp_t);
   ic.s_ic_prep.bind(prep.s_prep_t);
   ic.s_ic_ram.bind(ram.s_ram_t0);
   ic.s_ic_dma.bind(dma.s_dma_t);
   prep.s_prep_i.bind(ram.s_ram_t1);
	dma.s_dma_i.bind(s_vp_t1);

   dma.p_fifo(fifo);
   mlp.p_fifo(fifo);

   prep.p_out(p_out0);
   mlp.p_out(p_out1);

}

void VP::b_transport0(pl_t& pl, sc_time& delay)
{
	s_vp_i0->b_transport(pl, delay);
}
void VP::b_transport1(pl_t& pl, sc_time& delay)
{
	s_vp_i1->b_transport(pl, delay);
}
