#include "interop.decl.h"
#include <stdlib.h>
#include "mpi-interoperate.h"
#include <mpi.h>

CProxy_Main mainProxy;
CProxy_NG ng;

class Main : public CBase_Main
{
    public:
    Main(CkArgMsg *m)
    {
        mainProxy = thisProxy;
        ng = CProxy_NG::ckNew();
    }
    void done(int done)
    {
        CharmLibExit();
        return;
    }
};

class NG : public CBase_NG
{
    private:
    int mype;
    int numpes;
    int argc;
    char** argv;

    public:
    NG()
    {
        thisProxy[thisIndex].run_mpi();
    }
    void run_mpi()
    {
        mype = CkMyPe();
        numpes = CkNumPes();
        int rank;
        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank); //get my mpi rank
        ckout << "The MPI Rank of PE " << mype << " is " << rank << endl;
        //if i'm not the last pe, send a 0 to the next pe
        if(mype != numpes-1)
        {
            int send_val = 0;
            ckout << "PE " << mype << " sending to PE " << mype + 1 << endl;
            MPI_Send(&send_val, 1, MPI_INT, mype + 1, 0, MPI_COMM_WORLD);
        }
        //if i'm not the first pe, receive the send value from the previous pe
        //increment, and send back
        if(mype != 0)
        {
            int recv_val;
            MPI_Status status;
            MPI_Recv(&recv_val, 1, MPI_INT, mype - 1, 0, MPI_COMM_WORLD, &status);
            ckout << "PE " << mype << " received from PE " << mype - 1 << endl;
            recv_val++;
            MPI_Send(&recv_val, 1, MPI_INT, mype - 1, 1, MPI_COMM_WORLD);
            ckout << "PE " << mype << " sending to PE " << mype - 1 << endl;
        }
        //if i'm not the last pe, receive the incremented value from the previous pe
        //print it, then call mpiWorkDone
        if(mype != numpes-1)
        {
            int recv_val;
            MPI_Status status;
            MPI_Recv(&recv_val, 1, MPI_INT, mype + 1, 1, MPI_COMM_WORLD, &status);
            ckout << "Value received on PE " << mype << " is " << recv_val << endl;
        }
        thisProxy[thisIndex].mpi_work_done();
    }
    void mpi_work_done()
    {
        if(mype != numpes-1) thisProxy[thisIndex+1].charm_message();
        if(mype==0) thisProxy[thisIndex].charm_message();
    }
    void charm_message()
    {
        int done = 1;
        CkCallback cb(CkReductionTarget(Main, done), mainProxy);
        contribute(sizeof(int), &done, CkReduction::sum_int, cb);
    }
};

#include "interop.def.h"