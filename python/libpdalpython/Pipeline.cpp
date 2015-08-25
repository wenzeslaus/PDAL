#include "Pipeline.hpp"
#ifdef PDAL_HAVE_LIBXML2
#include <pdal/XMLSchema.hpp>
#endif


namespace libpdalpython
{

Pipeline::Pipeline(std::string const& xml)
    : m_xml(xml)
    , m_schema("")
{

}

void Pipeline::execute()
{
    pdal::PipelineManager manager(-1); // no progress reporting
    pdal::PipelineReader reader(manager, false, 0 );
    std::stringstream strm;
    strm << m_xml;
    bool isWriter = reader.readPipeline(strm);
    manager.execute();
#ifdef PDAL_HAVE_LIBXML2
    pdal::XMLSchema schema(manager.pointTable().layout());
    m_schema = schema.xml();
#endif

    pdal::PipelineWriter writer(manager);
    strm.str("");
    writer.writePipeline(strm);
    m_xml = strm.str();

}

std::string Pipeline::getSchema() const
{

    return m_schema;
}
} //namespace libpdalpython

