#include "D_spec_calc.h"
#include  "eq_errors.h"
#include  "eq_fct_errors.h"

D_spec_calc::D_spec_calc(const char* pn, D_spectrum* sp, D_int* startCh, D_int* endCh, D_float* mean, D_float* rms, D_float* onech, EqFct* eq) : D_iiii(pn, eq)
{
    spec_ = sp;
    startCh_ = startCh;
    endCh_ = endCh;
    mean_ = mean;
    rms_ = rms;
    onech_ = onech;
}

float D_spec_calc::evaluate()
{
    float f = 0.0, f1 = 0.0, tmp = 0.0;
    float rms = 0, mean = 0, sqsum = 0;

    int i1 = value()->i1_data;
    int i2 = value()->i2_data;
    int i3 = value()->i3_data;
    int i4 = value()->i4_data;

    if ( i1 < 0 ) i1 = 0;
    if ( i2 < 0 ) i2 = 0;
    if ( i3 < 0 ) i3 = 0;
    if ( i4 < 1 ) i4 = 1;

    if ( rms_ && mean_ && startCh_ && endCh_ && onech_ )
    {
        if ( startCh_->value() < endCh_->value() )
        {
            int st = startCh_->value(), en = endCh_->value();
            float fl;
            for ( int i = st; i < en; i++ )
            {
                fl = spec_->read_spectrum(i);
                mean += fl;
                sqsum += fl*fl;
            }
            mean /= (en - st);
            rms = sqrt(fabs(sqsum / (en - st) - mean * mean));
            mean_->set_value(mean);
            rms_->set_value(rms);
            onech_->set_value(spec_->read_spectrum(st));
        }
        else
        {
            mean_->set_value(0);
            rms_->set_value(0);
            onech_->set_value(0);
        }

    }


    switch ( i1 )
    {
        case 0: f = 0.0;
            break;
        case 1: // mean

            for ( int i = i2; i < i2 + i3; i += i4 )
            {
                f += spec_->read_spectrum(i);
            }
            f = f / (i3 / i4);
            break;
        case 2: // minimum
            f = 1.0e10;
            for ( int i = i2; i < i2 + i3; i += i4 )
            {
                tmp = spec_->read_spectrum(i);
                if ( f >= tmp ) f = tmp;
            }
            break;
        case 3: // maximum
            f = -1.0e10;
            for ( int i = i2; i < i2 + i3; i += i4 )
            {
                tmp = spec_->read_spectrum(i);
                if ( f <= tmp ) f = tmp;
            }
            break;
        case 4: // RMS
            for ( int i = i2; i < i2 + i3; i += i4 )
            {
                f1 = spec_->read_spectrum(i);
                f += f1*f1;
            }
            f = sqrt(f / i3);
            break;
        case 5: // amplitude
            f = 1.0e10;
            f1 = -1.0e10;
            for ( int i = i2; i < i2 + i3; i += i4 )
            {
                tmp = spec_->read_spectrum(i);
                if ( f >= tmp ) f = tmp; // min
                if ( f1 <= tmp ) f1 = tmp; // max
            }
            f = f1 - f;
            break;
        default: break;
    }
    return f;
};

// ========================= CLASS D_Spectrum =====================

D_Spectrum::D_Spectrum(const char *pn, u_int maxl, EqFct *ef, bool store) :
D_spectrum(pn, maxl, ef, store)
{
}

void D_Spectrum::get(EqAdr *ea, EqData *src, EqData *res, EqFct *)
{
    if ( src->type() == DATA_TTII )
    { // get old spectrum
        int i1, i2;
        time_t tm1, tm2;
        src->get_ttii(&tm1, &tm2, &i1, &i2);
        if ( ar_spect_ )
        {
            char p_at_loc [ADDR_STRING_LENGTH];
            sprintf(p_at_loc, "%s/%s", ea->property(), ea->location());
            i1 = ar_spect_->SPEC_GetHistData(p_at_loc, tm1, res);
            if ( i1 ) res->error((int) ERR_ARCHIVE, "arvicher error");
        }
        else res->error((int) ERR_ARCHIVE, "arvicher error");
    }
    else if ( src->type() == DATA_IIII )
    {
        int buffNum, offset, increment, channNum;
        src->get_iiii(&buffNum, &offset, &increment, &channNum);
        //printf("from get 1: %f, %f, %d\n", (float)offset, (float)increment, channNum);
        // validation
        if(offset < 0) offset = 0;
        if(offset > SPECTRUM_LENGTH-1) offset = SPECTRUM_LENGTH-1;
        if(increment < 1) increment = 1;
        if(increment + offset > SPECTRUM_LENGTH) increment = SPECTRUM_LENGTH-offset;
        if(channNum<1)channNum=1;
        if(offset+channNum>SPECTRUM_LENGTH) channNum=SPECTRUM_LENGTH-offset;

        SPECTRUM *sp = spectrum();
                float* tmp = new float[channNum];
                for ( int x = 0; x < channNum; x+=increment )
                {
                    tmp[x] = sp->d_spect_array.d_spect_array_val[x+offset];
                }
               res->set(
                        sp->comment.comment_val, // comment
                        sp->tm, // time
                        (float)offset, // start
                        (float)increment, // increment
                        sp->status, // status
                        tmp, // float*
                        channNum // len
                        );
               delete [] tmp;
    }
    else res->set(spectrum());
}
