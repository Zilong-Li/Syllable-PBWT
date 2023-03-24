#include "SyllablePBWT.h"
#include "log.hpp"

int main(int argc, char * argv[])
{

    // ========= helper message and parameters parsing ============================
    const std::vector<std::string> args(argv + 1, argv + argc);
    if(argc <= 1 || args[0] == "-h" || args[0] == "-help")
    {
        std::cout << "Author: Zilong-Li (zilong.dk@gmail.com)\n"
                  << "Usage example:\n"
                  << "     " + (std::string)argv[0]
                         + " -panel panel.vcf.gz -save mspbwt.bin -region chr22 -samples '^HG00103'\n"
                  << "     " + (std::string)argv[0]
                         + " -query panel.vcf.gz -load mspbwt.bin -region chr22 -samples HG00103\n"
                  << "     " + (std::string)argv[0] + " -load mspbwt.bin -region chr22 -randz 1 -k 3\n"
                  << "\nOptions:\n"
                  << "     -panel      vcf/bcf file for reference panel\n"
                  << "     -query      vcf/bcf file to query\n"
                  << "     -nindices   number of sets of mspbwt indices\n"
                  << "     -save       save mspbwt indicies as binary file\n"
                  << "     -load       load mspbwt indicies from binary file\n"
                  << "     -region     chromosome to be included, bcftools-like format\n"
                  << "     -samples    sample id to be included, bcftools-like format\n"
                  << "     -randz      random z to be inserted or not [0]\n"
                  << "     -l          min number of matching sites\n"
                  << std::endl;
        return 1;
    }
    std::string vcfpanel, vcfquery, binfile, samples = "-", region = "";
    int L{0}, nindices{1}, randz{0};
    for(size_t i = 0; i < args.size(); i++)
    {
        if(args[i] == "-panel") vcfpanel = args[++i];
        if(args[i] == "-query") vcfquery = args[++i];
        if(args[i] == "-save") binfile = args[++i];
        if(args[i] == "-load") binfile = args[++i];
        if(args[i] == "-region") region = args[++i];
        if(args[i] == "-samples") samples = args[++i];
        if(args[i] == "-nindices") nindices = stoi(args[++i]);
        if(args[i] == "-l") L = stoi(args[++i]);
        if(args[i] == "-randz") randz = stoi(args[++i]);
    }

    // ========= core calculation part ===========================================

    Logger cao("mspbwt.log");

    SyllablePBWT<unsigned long long> msp;
    if(!vcfpanel.empty())
    {
        msp.build(vcfpanel, samples, region);
        cao.print("Haplotypes:", msp.M, ", Markers:", msp.N, ", Grids:", msp.n, ", B:", msp.B);
        if(!binfile.empty()) msp.save(binfile);
    }
    else
    {
        if(!binfile.empty())
            msp.load(binfile);
        else
            throw invalid_argument("binary file with mspbwt indicies does not exist!\n");
        cao.print("Haplotypes:", msp.M, ", Markers:", msp.N, ", Grids:", msp.n, ", B:", msp.B);
        IntMapU haplens, hapends;
        if(!vcfquery.empty())
        {
            msp.query(vcfquery, samples, region, L, haplens, hapends);
            for(auto const & [hap, len] : haplens)
            {
                cao.print(hap, "\t", len);
            }
        }
    }

    return 0;
}
