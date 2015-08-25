#include <pdal/PipelineReader.hpp>
#include <pdal/PipelineManager.hpp>
#include <pdal/PipelineWriter.hpp>
#include <pdal/util/FileUtils.hpp>

#include <string>

namespace libpdalpython
{

class Pipeline {

public:
    Pipeline(std::string const& xml);
    ~Pipeline(){};

    void execute();
    inline std::string getXML() const { return m_xml; }
    std::string getSchema() const;

private:
    std::string m_xml;
    std::string m_schema;

};

}
