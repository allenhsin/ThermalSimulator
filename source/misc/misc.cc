
#include <cassert>
#include <vector>
#include <math.h>

#include "source/misc/args_parser.h"
#include "source/misc/misc.h"
#include "config_file.hpp"

using std::vector;
using std::string;

namespace Thermal
{
    void Misc::setConfig(string config_file, config::Config*& cfg_ref, int argc, char** argv)
    {
        // string vector to store parsed command line config arguments
        string_vec  args;

        // create command line argument parser
        ArgsParser* args_parser = new ArgsParser();
        // create config instance 
        config::ConfigFile* cfg = new config::ConfigFile();

        // parse command line arguments into args and config_file
        args_parser->parseArgs(args, config_file, argc, argv);
        // load config_file config settings into cfg
        cfg->load(config_file);
        // override cfg with command line arguments (if any)
        args_parser->handleArgs(args, *cfg);
        // put cfg into simulator
        cfg_ref = cfg;

        delete args_parser;
    }

    void Misc::initDouble1DVector(vector<double>& vec_ref, int dim_1)
    {
        assert(dim_1>0);
        vec_ref.clear();
        vec_ref.resize(dim_1);
    }

    void Misc::initDouble2DVector(vector< vector<double> >& vec_ref, int dim_1, int dim_2)
    {
        assert(dim_1>0 && dim_2>0);
        vec_ref.clear();
        vec_ref.resize(dim_1);
        for(int i=0; i<dim_1; ++i)
            vec_ref[i].resize(dim_2);
    }

    void Misc::initDouble3DVector(vector< vector< vector<double> > >& vec_ref, int dim_1, int dim_2, int dim_3)
    {
        int i, j;
        assert(dim_1>0 && dim_2>0 && dim_3>0);
        vec_ref.clear();
        vec_ref.resize(dim_1);
        for(i=0; i<dim_1; ++i)
        {
            vec_ref[i].resize(dim_2);
            for(j=0; j<dim_2; ++j)
                vec_ref[i][j].resize(dim_3);
        }
    }

    void Misc::initInt1DVector(vector<int>& vec_ref, int dim_1)
    {
        assert(dim_1>0);
        vec_ref.clear();
        vec_ref.resize(dim_1);
    }

    void Misc::initInt2DVector(vector< vector<int> >& vec_ref, int dim_1, int dim_2)
    {
        assert(dim_1>0 && dim_2>0);
        vec_ref.clear();
        vec_ref.resize(dim_1);
        for(int i=0; i<dim_1; ++i)
            vec_ref[i].resize(dim_2);
    }

    bool Misc::eq(double x, double y)
    { return (fabs(x-y) <  DELTA); }

    void Misc::swapDoubleValue (double *a, double *b)
    {
        double t = *a;
        *a = *b;
        *b = t;
    }

    void Misc::swapIntValue (int *a, int *b)
    {
        int t = *a;
        *a = *b;
        *b = t;
    }

} // namespace Thermal

