#include "iprocess.h"

namespace loss
{
    IProcess::IProcess(std::ostream *stdout) :
        _stdout(stdout)
    {

    }

    std::ostream *IProcess::stdout() const
    {
        return _stdout;
    }
    void IProcess::stdout(std::ostream *value)
    {
        _stdout = value;
    }
}
