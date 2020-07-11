#include    "protocolformat.h"

ProtocolFormat::ProtocolFormat()
{
    m_Frame_index = 0;

        }

ProtocolFormat::~ProtocolFormat()
{

}

quint16 ProtocolFormat::get_Protocol_frame_index()
{
    return m_Frame_index;
}

void ProtocolFormat::setProtocolFrameIndex(quint16 val)
{
        m_Frame_index = val;
}
