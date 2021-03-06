////////////////////////////////////////////////////////////
//                                                        //
// This file is part of the VRPH software package for     //
// generating solutions to vehicle routing problems.      //
// VRPH was developed by Chris Groer (cgroer@gmail.com).  //
//                                                        //
// (c) Copyright 2010 Chris Groer.                        //
// All Rights Reserved.  VRPH is licensed under the       //
// Common Public License.  See LICENSE file for details.  //
//                                                        //
////////////////////////////////////////////////////////////

#include "VRPH.h"



bool SwapEnds::move(VRP *V, int a, int v)
{
    ///
    /// This function takes the routes corresponding to nodes a and v
    /// and swaps the ends of these routes following a and v respectively.
    /// Example:  VRPH_DEPOT-i-a-j-k-l-VRPH_DEPOT and VRPH_DEPOT-t-u-v-x-y-z-VRPH_DEPOT becomes
    /// VRPH_DEPOT-i-a-x-y-z-VRPH_DEPOT and VRPH_DEPOT-t-u-v-j-k-l-VRPH_DEPOT.
    /// If the proposed move is feasible, all solution modifications
    /// are made, and the function returns true.  Returns false if the move
    /// is infeasible.
    ///
    

    VRPMove M;

    if(evaluateHelper(V,a,v,&M)==false)
        return false;    // move is infeasible

    int current_node, a_start, a_end, v_start, v_end;
    int route_after_a, route_before_a, route_before_v, route_after_v;
    int  n, u, b;


#if SWAP_ENDS_DEBUG>0
        printf("Calling swap ends(%d,%d)\n",a,v);
#endif
    

    if(a==VRPH_DEPOT || v==VRPH_DEPOT)
        report_error("%s: Swap ends called with depot; Move doesn't make sense\n",__FUNCTION__);
    

    n = V->num_nodes;

    if(V->route_num[a] == V->route_num[v])
        report_error("%s: swap ends called with a and v in same route!\n",__FUNCTION__);

#if SWAP_VERIFY
    V->verify_routes("SWAP_ENDS::prior to move\n");
#endif

    V->update(&M);

    a_end = V->route[V->route_num[a]].end;
    a_start = V->route[V->route_num[a]].start;
    
    route_after_a = V->route_num[-V->next_array[a_end]];
    route_before_a = V->route_num[-V->pred_array[a_start]];

    v_end = V->route[V->route_num[v]].end;
    v_start = V->route[V->route_num[v]].start;
    
    route_after_v = V->route_num[-V->next_array[v_end]];
    route_before_v = V->route_num[-V->pred_array[v_start]];
    
    u = V->next_array[v];
    b = V->next_array[a];

    if(u==VRPH_DEPOT || b==VRPH_DEPOT)
    {
        report_error("%s: u=0 or b=0 in swap_ends\n",__FUNCTION__);

    }

    if(u>0 && b>0)
    {
        V->next_array[a]=u;
        V->pred_array[u]=a;
        V->next_array[v]=b;
        V->pred_array[b]=v;
    }
    else
    {
        if(u>0 && b<0)
        {
            V->next_array[a]=u;
            V->pred_array[u]=a;
            V->next_array[v]=b;
            V->pred_array[-b]=-v;
        }
        else
        {

            if(u<0 && b>0)
            {
                V->next_array[a]=u;
                V->pred_array[-u]=-a;
                V->next_array[v]=-b;
                V->pred_array[b]=v;
            }
            else
                report_error("%s: Reached strange place...\n",__FUNCTION__);
        }
    }

    

    // Now update the start, end, length, and load info for V->route_num[a] and V->route_num[v]

    
    V->route[V->route_num[a]].start = a_start;
    V->route[V->route_num[a]].end = v_end;

    
    V->route[V->route_num[v]].start = v_start;
    V->route[V->route_num[v]].end = a_end;

    // Now update the route_num-looping...
    
    current_node = a;
    while(current_node > 0)
    {
        V->route_num[current_node] = V->route_num[a];
        current_node = V->next_array[current_node];
        
    }

    current_node = v;
    while(current_node > 0)
    {
        V->route_num[current_node] = V->route_num[v];
        current_node = V->next_array[current_node];

    }

    // Now we have to update the routes following the modified a and v routes

    // Get the new start and end values 
    a_start = V->route[V->route_num[a]].start;
    v_start = V->route[V->route_num[v]].start;
    a_end = V->route[V->route_num[a]].end;
    v_end = V->route[V->route_num[v]].end;


    if(route_after_a != V->route_num[v] && route_after_v != V->route_num[a])
    {
        

        // Simple case - just switch the two
        if(route_after_a != 0 && route_after_v!=0)
        {
            
            V->next_array[a_end] = -V->route[route_after_a].start;
            V->pred_array[V->route[route_after_a].start] = -a_end;

            V->next_array[v_end] = -V->route[route_after_v].start;
            V->pred_array[V->route[route_after_v].start] = -v_end;

            return true;
        }

        if(route_after_a == 0)
        {
            V->next_array[a_end] = VRPH_DEPOT;
            V->pred_array[VRPH_DEPOT] = -a_end;

            V->next_array[v_end] = -V->route[route_after_v].start;
            V->pred_array[V->route[route_after_v].start] = -v_end;


            return true;
        }

        if(route_after_v == 0)
        {
            
            V->next_array[v_end] = VRPH_DEPOT;
            V->pred_array[VRPH_DEPOT] = -v_end;

            V->next_array[a_end] = -V->route[route_after_a].start;
        
            V->pred_array[V->route[route_after_a].start] = -a_end;

            return true;
        }
    }

    
    if(route_after_a == V->route_num[v] && route_after_v == V->route_num[a])
    {
        // If we don't change anything here, then the new V->route_num[v] will point to itself!
        // To fix this, make the new_v_route point 
        
        V->next_array[a_end] = -v_start;
        V->pred_array[v_start] = -a_end;

        V->next_array[v_end] = -a_start;
        V->pred_array[a_start] = -v_end;

        return true;

        
    }

    
    if(route_after_a == V->route_num[v])
    {
        
        // If we don't change anything here, then the new V->route_num[v] will point to itself!
        
        V->next_array[a_end] = -v_start;
        V->pred_array[v_start] = -a_end;

        if(route_after_v != VRPH_DEPOT)
        {
            V->next_array[v_end] = -V->route[route_after_v].start;
            V->pred_array[V->route[route_after_v].start] = -v_end;
        }
        else
        {
            V->next_array[v_end] = VRPH_DEPOT;
            V->pred_array[VRPH_DEPOT] = -v_end;
        }

#if SWAP_ENDS_VERIFY
        V->verify_routes("SwapEnds 5\n");
#endif

        return true;

        
    }

    if(route_after_v == V->route_num[a])
    {        
            
        // If we don't change anything here, then the new V->route_num[a] will point to itself!
        
        V->next_array[v_end] = -a_start;
        V->pred_array[a_start] = -v_end;

        if(route_after_a!=VRPH_DEPOT)
        {
            V->next_array[a_end] = -V->route[route_after_a].start;
            V->pred_array[V->route[route_after_a].start] = -a_end;
        }
        else
        {
            V->next_array[a_end] = VRPH_DEPOT;
            V->pred_array[VRPH_DEPOT] = -a_end;
        }



        return true;


    }

    report_error("%s: should never be here\n",__FUNCTION__);

    return false;


}
