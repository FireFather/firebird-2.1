#include "firebird.h"
#ifdef BENCHMARK

void BenchMark(typePos* Position, const char* GoString)
    {
	    uint64_t TotalTime = 0, TotalNodes = 0;

    for ( int i = 0; i < 16; i++ )
        {
    	char GO_String[1024];
    	char String[1024];
    	sprintf(String, "%s %s\n", "fen", BenchmarkPositions[i]);
        InitPosition(Position, String);
        strcpy(GO_String, GoString);
        InitSearch(Position, GO_String);
        const uint64_t C = GetClock();
        Search(Position);
        const uint64_t TIME = GetClock() - C;
        uint64_t Nodes = 0;

		for ( int cpu = 0; cpu < NumThreads; cpu++ )
			for ( int rp = 0; rp < RPperCPU; rp++ )
				Nodes += RootPosition[cpu][rp].nodes;
        Send("Position %d: Nodes: %lld Time: %lldms\n", 1 + i, Nodes, TIME / 1000);
        TotalNodes += Nodes;
        TotalTime += TIME;
        }
    Send("Total Nodes: %lld Time: %lldms\n", TotalNodes, TotalTime / 1000);
    Send("Total NPS: %lld\n", TotalNodes * 1000 / TotalTime * 1000);
    }
#endif
