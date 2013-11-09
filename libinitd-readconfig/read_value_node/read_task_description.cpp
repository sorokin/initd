#include "read_task_description.h"

#include "lexer/text_range.h"
#include "lexer/error_tag.h"
#include "lexer/lexer.h"
#include "parser/top_level_node.h"
#include "parser/task_node.h"
#include "parser/value_node.h"
#include "parser/run_level_node.h"
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
    void read_tasks(top_level_node const& node, error_tag_sink& esink, task_descriptions& res)
    {
        std::multimap<std::string, task_node*> tasks;

        for (task_node_sp const& tnode_sp : node.get_tasks())
        {
            tasks.insert(std::make_pair(tnode_sp->get_name().get_text(), tnode_sp.get()));
        }

        report_duplicates(tasks, [&](task_node* node) { esink.push(error_tag(node->get_name().get_range(), "duplicate task definition")); });

        for (auto const& p : tasks)
        {
            auto t = read_task(*p.second, esink);
            if (t)
                res.create_task_description(p.first, *t);
            else
                res.create_task_description(p.first, null_task_data());
        }
    }

    void resolve_task(identifier_token const& task_name_node,
                      task_descriptions const& tds,
                      error_tag_sink& esink,
                      std::vector<task_description*>& res)
    {
        std::string task_name = task_name_node.get_text();

        auto i = tds.get_task_by_name().find(task_name);
        if (i == tds.get_task_by_name().end())
        {
            std::stringstream ss;
            ss << "task \"" << task_name << "\" is not declared";

            esink.push(error_tag(task_name_node.get_range(), ss.str()));
            return;
        }

        res.push_back(i->second);
    }

    void read_dependecies(top_level_node const& node,
                          error_tag_sink& esink,
                          task_descriptions& res)
    {
        for (dependency_node_sp const& dnode_sp : node.get_dependencies_decls())
        {
            dependency_node const& dnode = *dnode_sp;

            std::string dependant = dnode.get_dependant().get_text();

            auto dependant_i = res.get_task_by_name().find(dependant);
            if (dependant_i == res.get_task_by_name().end())
            {
                std::stringstream ss;
                ss << "task \"" << dependant << "\" is not declared";

                esink.push(error_tag(dnode.get_dependant().get_range(), ss.str()));
                continue;
            }

            std::vector<task_description*>& dependencies = dependant_i->second->get_dependencies();

            for (identifier_token_sp const& inode_sp : dnode.get_dependencies())
                resolve_task(*inode_sp, res, esink, dependencies);
        }
    }

    void read_run_levels(top_level_node const& node,
                         error_tag_sink& esink,
                         task_descriptions& res)
    {
        std::multimap<std::string, run_level_node*> run_levels;

        for (run_level_node_sp const& rlnode_sp : node.get_run_levels())
        {
            run_levels.insert(std::make_pair(rlnode_sp->get_name().get_text(), rlnode_sp.get()));
        }

        report_duplicates(run_levels, [&](run_level_node* node) { esink.push(error_tag(node->get_name().get_range(), "duplicate run level definition")); });

        for (auto const& p : run_levels)
        {
            std::vector<task_description*> requisites;

            for (identifier_token_sp const& dnode_sp : p.second->get_dependencies())
                resolve_task(*dnode_sp, res, esink, requisites);

            res.create_run_level(p.first, std::move(requisites));
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

    task_descriptions tds;
    read_tasks(*tree, esink, tds);
    read_dependecies(*tree, esink, tds);
    read_run_levels(*tree, esink, tds);

    if (!esink.get_errors().empty())
    {
        line_map lmap{whole_file};

        for (error_tag const& e : esink.get_errors())
        {
            print_error(error_stream, config_filename, lmap, e);
        }

        error_stream << std::endl;
    }

    return tds;
}
