#include "read_task_description.h"

#include "lexer/text_range.h"
#include "lexer/error_tag.h"
#include "lexer/lexer.h"
#include "parser/top_level_node.h"
#include "parser/task_node.h"
#include "parser/value_node.h"
#include "parser/parser.h"
#include "line_map/line_map.h"
#include "line_map/print_error.h"

#include "read_task_data.h"
#include "task_description.h"
#include "task_descriptions.h"

#include "make_unique.h"
#include "file_descriptor.h"

namespace
{
    std::multimap<std::string, task_description_sp> read_tasks(top_level_node const& node, error_tag_sink& esink)
    {
        std::multimap<std::string, task_node*> tasks;

        for (task_node_sp const& tnode_sp : node.get_tasks())
        {
            tasks.insert(std::make_pair(tnode_sp->get_name().get_text(), tnode_sp.get()));
        }

        report_duplicates(tasks, [&](task_node* node) { esink.push(error_tag(node->get_name().get_range(), "duplicate task definition")); });

        std::multimap<std::string, task_description_sp> tasks2;

        for (auto const& p : tasks)
        {
            auto t = read_task(*p.second, esink);
            if (t)
                tasks2.insert(std::make_pair(p.first, make_unique<task_description>(p.first, *t)));
            else
                tasks2.insert(std::make_pair(p.first, make_unique<task_description>(p.first, null_task_data())));
        }

        return tasks2;
    }

    void read_dependecies(top_level_node const& node,
                          error_tag_sink& esink,
                          std::multimap<std::string, task_description_sp>& tasks)
    {
        for (dependency_node_sp const& dnode_sp : node.get_dependencies_decls())
        {
            dependency_node const& dnode = *dnode_sp;

            std::string dependant = dnode.get_dependant().get_text();

            auto dependant_i = tasks.find(dependant);
            if (dependant_i == tasks.end())
            {
                std::stringstream ss;
                ss << "task \"" << dependant << "\" is not declared";

                esink.push(error_tag(dnode.get_dependant().get_range(), ss.str()));
                continue;
            }

            std::vector<task_description*>& dependencies = dependant_i->second->get_dependencies();

            for (identifier_token_sp const& inode_sp : dnode.get_dependencies())
            {
                std::string dependency = inode_sp->get_text();

                auto dependency_i = tasks.find(dependency);
                if (dependency_i == tasks.end())
                {
                    std::stringstream ss;
                    ss << "task \"" << dependency << "\" is not declared";

                    esink.push(error_tag(inode_sp->get_range(), ss.str()));
                    continue;
                }

                dependencies.push_back(dependency_i->second.get());
            }

        }
    }
}

task_descriptions read_descriptions(std::string const& config_filename, std::ostream& error_stream)
{
    std::vector<char> fdata = sysapi::read_entire_file(config_filename);

    text_range whole_file{fdata.data(), fdata.data() + fdata.size()};
    error_tag_sink esink;
    lexer lex{whole_file, esink};
    top_level_node_sp tree = parse_file(lex, esink);

    line_map lmap{whole_file};

    auto tasks = read_tasks(*tree, esink);
    read_dependecies(*tree, esink, tasks);

    if (!esink.get_errors().empty())
    {
        for (error_tag const& e : esink.get_errors())
        {
            print_error(error_stream, config_filename, lmap, e);
        }

        error_stream << std::endl;
    }

    task_descriptions tds;

    for (auto& t : tasks)
        tds.insert_task_description(std::move(t.second));

    return tds;
}
