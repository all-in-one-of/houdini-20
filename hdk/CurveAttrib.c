// calculate curve's tangent or N

#include <UT/UT_DSOVersion.h>
#include <SYS/SYS_Math.h>
#include <GU/GU_Detail.h>
#include <GEO/GEO_AttributeHandle.h>
#include <GEO/GEO_Primitive.h>
#include <PRM/PRM_Include.h>
#include <OP/OP_Director.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <GA/GA_AIFTuple.h>
#include <UT/UT_Vector3.h>
#include "CurveAttrib.h"

using namespace HDK_Sample;

void
newSopOperator(OP_OperatorTable *table)
{
     table->addOperator(new OP_Operator("curve_attrib",
                    "Curve Attrib",
                     CurveAttrib::myConstructor,
                     CurveAttrib::myTemplateList,
                     1,
                     1,
                     0));
}

PRM_Template
CurveAttrib::myTemplateList[] = {
    PRM_Template(),
};


OP_Node *
CurveAttrib::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new CurveAttrib(net, name, op);
}

CurveAttrib::CurveAttrib(OP_Network *net, const char *name, OP_Operator *op)
    : SOP_Node(net, name, op)
{
}

CurveAttrib::~CurveAttrib()
{
}

OP_ERROR
CurveAttrib::cookMySop(OP_Context &context)
{
    // Before we do anything, we must lock our inputs.  Before returning,
    // we have to make sure that the inputs get unlocked.
    if (lockInputs(context) >= UT_ERROR_ABORT)
    return error();

    // Duplicate input geometry
    duplicateSource(0, context);

    // fpreal frame = OPgetDirector()->getChannelManager()->getSample(context.getTime());

    GA_WOAttributeRef tangentRef;
    tangentRef = gdp->addFloatTuple(GA_ATTRIB_POINT, "tangent", 3);
    const GA_AIFTuple *tangentAIF = tangentRef.getAIFTuple();
    GA_RWHandleV3 Phandle(gdp->findAttribute(GA_ATTRIB_POINT, "P"));
    GEO_Primitive *prim;
    GEO_AttributeHandle     gah;

    GA_FOR_ALL_PRIMITIVES(gdp, prim)
    {
        GA_Range ptrange = prim->getPointRange();
        GA_Size npt = prim->getVertexCount()-1;
        GA_Offset lastpt = prim->getPointOffset(npt);
        GA_Offset firstpt = prim->getPointOffset(0);
        
        bool curve_is_closed = false;
        fpreal area = prim->calcArea();
        if (!SYSalmostEqual(area, 0.0)) {
            curve_is_closed = true;
        }

        for (GA_Iterator iterpt(ptrange.begin()); !iterpt.atEnd(); ++iterpt)
        {
            GA_Offset ptoff = iterpt.getOffset();
            UT_Vector3 tangent;
            UT_Vector3 Pcurr;
            UT_Vector3 Pnext;

            if ((ptoff==lastpt) && curve_is_closed) {
                Pnext = Phandle.get(firstpt);
                Pcurr = Phandle.get(ptoff);
                tangent = Pnext-Pcurr; //circular
            }
            else if ((ptoff==lastpt) && !curve_is_closed) {
                Pnext = Phandle.get(ptoff-1);
                Pcurr = Phandle.get(ptoff);
                tangent = Pcurr-Pnext; //backward

            }
            else {
                Pnext = Phandle.get(ptoff+1);
                Pcurr = Phandle.get(ptoff);
                tangent = Pnext-Pcurr;
            }

            tangent.normalize();
            tangentAIF->set(tangentRef.getAttribute(), ptoff, tangent.data(), 3);
        }
    }

    gdp->notifyCache(GU_CACHE_ALL);
    unlockInputs();
    return error();
}

