#include "count_binary.h"
#include <math.h>

#define BUFF_SIZE 100

/* A "loop counter" variable. */
static alt_u16 in;
float buffer [BUFF_SIZE];

static void load_buffer()
{
	int i;
	for (i = 0; i < BUFF_SIZE; i++)
	{
		in = IORD_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE);
		buffer[i] = in;
		//IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, in);
	}
}

void fft(float* data, unsigned long nn)
{
    unsigned short n, mmax, m, j, istep, i;
    float wtemp, wr, wpr, wpi, wi, theta;
    float tempr, tempi, swp;

    // reverse-binary reindexing
    n = nn<<1;
    j=1;
    for (i=1; i<n; i+=2) {
        if (j>i) {

            swp = data[j-1];
            data[j-1] = data[i-1];
            data[i-1] = swp;

            swp = data[j];
            data[j] = data[i];
            data[i] = swp;
        }
        m = nn;
        while (m>=2 && j>m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    };

    // here begins the Danielson-Lanczos section
    mmax=2;
    while (n>mmax) {
        istep = mmax<<1;
        theta = -(2*M_PI/mmax);
        wtemp = sin(0.5*theta);
        wpr = -2.0*wtemp*wtemp;
        wpi = sin(theta);
        wr = 1.0;
        wi = 0.0;
        for (m=1; m < mmax; m += 2) {
            for (i=m; i <= n; i += istep) {
                j=i+mmax;
                tempr = wr*data[j-1] - wi*data[j];
                tempi = wr * data[j] + wi*data[j-1];

                data[j-1] = data[i-1] - tempr;
                data[j] = data[i] - tempi;
                data[i-1] += tempr;
                data[i] += tempi;
            }
            wtemp=wr;
            wr += wr*wpr - wi*wpi;
            wi += wi*wpr + wtemp*wpi;
        }
        mmax=istep;
    }
}

int main(void)
{
    int i;

    while( 1 ) 
    {
        usleep(10000);
        printf("loading buffer with values....\n");
        load_buffer();
        printf("loaded buffer\n");
        fft(buffer, BUFF_SIZE);
        printf("running fft\n");
        printf("value: %d\n", (int)buffer[0]);
    }
    return 0;
}
