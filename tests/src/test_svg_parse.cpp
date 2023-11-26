/*! \file
    \brief Test SVG parser
*/

#include "umba/umba.h"
#include "umba/simple_formatter.h"
#include "umba/string_plus.h"

#include "marty_fs_adapters/simple_file_api.h"

#include "marty_dc_impl_win32/gdi_draw_context.h"
#include "marty_dc_impl_win32/gdiplus_draw_context.h"
#include <pugixml/pugixml.hpp>

// 
#include "marty_draw_context/svg.h"
#include "marty_draw_context/utils.h"

//
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <queue>
#include <deque>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <random>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <functional>





int main(int argc, char* argv[])
{
    using std::cerr;

    if (argc<2)
    {
        cerr << "No file name taken\n";
        return 1;
    }

    std::string fileName = argv[1];

    marty_fs_adapters::SimpleFileApi<std::string> fsApi;

    std::string svgText = fsApi.readFile( fileName );
    if (svgText.empty())
    {
        cerr << "Failed to read file: " << fileName << "\n";
        return 1;
    }

    // std::vector<std::uint8_t> svgDataVec ((const std::uint8_t*)svgText.data(), ((const std::uint8_t*)svgText.data())+svgText.size() );

    pugi::xml_document svgDoc;
    pugi::xml_parse_result result = svgDoc.load_buffer((const void*)svgText.c_str(), svgText.size());

    if (result)
    {
        //cerr << "XML [" << << "] parsed without errors, attr value: [" << genresConverterXmlDoc.child("node").attribute("attr").value() << "]\n\n";
    }
    else
    {
        //cerr << "XML parsed with errors, attr value: [" + svgDoc.child("node").attribute("attr").value() << "]\n";
        cerr << result.description() << std::string("\n"); //  + 
        cerr << "Error offset: " + result.offset << ", error at [..." << (svgText.c_str() + result.offset) << "\n";
    }


    pugi::xml_node svgNode = svgDoc.document_element();

    //using namespace marty_draw_context::svg;

    #if 0
    SizeWithDimensions imageSize;
    ViewBox imageViewBox;

    std::size_t nextConvertPos = 0;


    std::unordered_map< std::string, std::unordered_set<std::string> > nsPrefixes; // "http://www.w3.org/2000/svg" -> {"svg", ""}
    std::unordered_map< std::string, std::string >                     nsPrefixNamespaces; // "svg" -> http://www.w3.org/2000/svg
    
    

    {
        pugi::xml_attribute svgAttr = svgNode.first_attribute();
        for(; svgAttr; svgAttr=svgAttr.next_attribute())
        {
            auto attrParts = marty_draw_context::utils::simpleStringSplit(std::string(svgAttr.name()), ':', 2 /* nSplits */ );
    
            if (attrParts.empty())
            {
                continue;
            }
    
            
            if (attrParts[0]=="xmlns")
            {
                std::string nsPrefix;
                if (attrParts.size()>1)
                {
                    nsPrefix = attrParts[1];
                }
    
                std::string ns = svgAttr.value();
                nsPrefixes[ns].insert(nsPrefix);
                nsPrefixNamespaces[nsPrefix] = ns;
    
            }
            else if (attrParts.back()=="width")
            {
                imageSize.width   = SizeWithDimensions::valueFromString(svgAttr.value(), &nextConvertPos);
            }
            else if (attrParts.back()=="height")
            {
                imageSize.height  = SizeWithDimensions::valueFromString(svgAttr.value(), &nextConvertPos);
            }
            else if (attrParts.back()=="viewBox")
            {
                imageViewBox = ViewBox::fromString(svgAttr.value(), &nextConvertPos);
            }
        }
    }


    static const std::string svgNs = "http://www.w3.org/2000/svg";
    #endif

    try
    {
        //marty_draw_context::svg::Image 
        auto img = marty_draw_context::svg::Image::fromSvgXmlNode(svgNode);

        #if 0
        pugi::xml_node node = svgNode.first_child();
        for(; node; node=node.next_sibling())
        {
            std::string name;
            std::string ns = parseXmlTagName(nsPrefixNamespaces, node.name(), name);
            if (ns!=svgNs)
            {
                cerr << "- Not SVG node: " << node.name() << "\n" << std::flush;
            }
            else
            {
                if (Shape::isKnownShapeName(name))
                {
                    Shape shape = Shape::fromXmlNode(node, nsPrefixNamespaces);
                }
                else
                {
                    cerr << "!!! Unknown tag: " << name << "\n" << std::flush;
                }
                
            } // if

        } // for
        #endif
    }
    catch(const std::exception &e)
    {
        cerr << e.what() << "\n" << std::flush;
    }

    return 0;
}



