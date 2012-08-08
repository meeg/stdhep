#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdhep_mcfio.h>
#include <stdhep.h>
#include <TFile.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <string.h>


// takes input stdhep file, explodes each event into single-particle events, and writes to a new stdhep file
int main(int argc,char** argv)
{
	int nevhep;             /* The event number */
	int nhep;               /* The number of entries in this event */
	int isthep[NMXHEP];     /* The Particle id */
	int idhep[NMXHEP];      /* The particle id */
	int jmohep[NMXHEP][2];    /* The position of the mother particle */
	int jdahep[NMXHEP][2];    /* Position of the first daughter... */
	double phep[NMXHEP][5];    /* 4-Momentum, mass */
	double vhep[NMXHEP][4];    /* Vertex information */

	if (argc!=4) 
	{
		printf("<input stdhep filename> <output stdhep filename> <number of events per event>\n");
		return 1;
	}
	int n_events;
	int istream = 0;
	int ostream = 1;
	int ilbl;
	StdHepXdrReadInit(argv[1],n_events,istream);
	printf("Reading %d events from %s\n",n_events,argv[1]);

	int n_merge = atoi(argv[3]);
	printf("Writing to %s, %d events per event\n",argv[2],n_merge);
	StdHepXdrWriteOpen(argv[2],argv[2],n_events,ostream);
	StdHepXdrWrite(100,ostream);

	nevhep = 0;

	while (true) {
		nhep = 0;
		bool bunches_merged = true;
		for (int i=0;i<n_merge;i++)
		{
			do {
				StdHepXdrRead(&ilbl,istream);
			} while (ilbl==100);

			if (ilbl!=1)
			{
				printf("End of file\n");
				bunches_merged = false;
				break;
			}

			for (int i = 0;i<hepevt_.nhep;i++)
			{
				isthep[nhep+i] = hepevt_.isthep[i];
				idhep[nhep+i] = hepevt_.idhep[i];
				for (int j=0;j<2;j++) jmohep[nhep+i][j] = hepevt_.jmohep[i][j];
				for (int j=0;j<2;j++) jdahep[nhep+i][j] = hepevt_.jdahep[i][j];
				for (int j=0;j<5;j++) phep[nhep+i][j] = hepevt_.phep[i][j];
				for (int j=0;j<4;j++) vhep[nhep+i][j] = hepevt_.vhep[i][j];
			}
			nhep += hepevt_.nhep;
		}
		if (!bunches_merged) break;


		hepevt_.nhep = nhep;
		hepevt_.nevhep = nevhep;

		for (int i = 0;i<nhep;i++)
		{
			hepevt_.isthep[i] = isthep[i];
			hepevt_.idhep[i] = idhep[i];
			for (int j=0;j<2;j++) hepevt_.jmohep[i][j] = jmohep[i][j];
			for (int j=0;j<2;j++) hepevt_.jdahep[i][j] = jdahep[i][j];
			for (int j=0;j<5;j++) hepevt_.phep[i][j] = phep[i][j];
			for (int j=0;j<4;j++) hepevt_.vhep[i][j] = vhep[i][j];
		}
		StdHepXdrWrite(ilbl,ostream);
		nevhep++;
	}

	StdHepXdrWrite(200,ostream);
	StdHepXdrEnd(ostream);
	StdHepXdrEnd(istream);
}
