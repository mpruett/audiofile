#ifndef REBUFFER_H
#define REBUFFER_H

_AFmoduleinst _af_initfloatrebufferv2f (AFframecount nsamps, bool multiple_of);
_AFmoduleinst _af_initfloatrebufferf2v (AFframecount nsamps, bool multiple_of);

_AFmoduleinst _af_initint2rebufferv2f (AFframecount nsamps, bool multiple_of);
_AFmoduleinst _af_initint2rebufferf2v (AFframecount nsamps, bool multiple_of);

#endif /* REBUFFER_H */
