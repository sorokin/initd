#include "read_task_data.h"

std::multimap<std::string, property_node*> make_properties_map(struct_node const& cnode, error_tag_sink& esink)
{
    std::multimap<std::string, property_node*> properties_by_name;
    for (property_node_sp const& p : cnode.get_properties())
        properties_by_name.insert(std::make_pair(p->get_key().get_text(), p.get()));

    report_duplicates(properties_by_name, [&](property_node* p) { esink.push(error_tag(p->get_key().get_range(), "duplicate property declaration")); });

    return properties_by_name;
}

const char* read_value_node_impl<hostname_task_data>::get_type_name()
{
    return "sethostname";
}

bool read_value_node_impl<hostname_task_data>::can_read(value_node const& node)
{
    if (node.get_type() != value_node_type::struct_)
        return false;

    struct_node const& cnode = static_cast<struct_node const&>(node);
    return cnode.get_tag().get_text() == get_type_name();
}

boost::optional<hostname_task_data> read_value_node_impl<hostname_task_data>::read(value_node const& node, error_tag_sink& esink)
{
    assert(node.get_type() == value_node_type::struct_);

    struct_node const& cnode = static_cast<struct_node const&>(node);
    assert(cnode.get_tag().get_text() == get_type_name());

    std::multimap<std::string, property_node*> properties_by_name = make_properties_map(cnode, esink);

    hostname_task_data data;

    boost::optional<std::string> hostname1 = extract_property_from_map<std::string>(cnode, properties_by_name, "hostname", esink);
    if (!hostname1)
        return boost::none;

    data.hostname = *hostname1;
    data.no_restore = extract_property_from_map<bool>(cnode, properties_by_name, "no_restore", false, esink);

    return data;
}

boost::optional<hostname_task_data> read_value_node_impl<hostname_task_data>::value_for_pseudo_identifier(pseudo_identifier_value_node const&)
{
    return boost::none;
}

const char* read_value_node_impl<cmd_line>::get_type_name()
{
    return "cmd_line";
}

bool read_value_node_impl<cmd_line>::can_read(value_node const& node)
{
    return node.get_type() == value_node_type::string;
}

boost::optional<cmd_line> read_value_node_impl<cmd_line>::read(value_node const& node, error_tag_sink& esink)
{
    assert(node.get_type() == value_node_type::string);

    string_node const& cnode = static_cast<string_node const&>(node);
    std::string const& value = cnode.get_token().get_value();

    std::vector<std::string> split;
    std::string::const_iterator i = value.cbegin();
    for (;;)
    {
        i = std::find_if(i, value.cend(), [](char c) { return !is_ascii_whitespace(c); });
        if (i == value.cend())
            break;

        std::string arg;

        for (;;)
        {
            char c = *i;
            if (c == '\'')
            {
                ++i;
                auto j = std::find_if(i, value.cend(), [](char c) { return c == '\''; });
                if (j == value.cend())
                {
                    esink.push(error_tag(node.get_range(), "unmatched \'"));
                    return boost::none;
                }

                arg.insert(arg.end(), i, j);
                ++j;
                i = j;
            }
            else if (c == '\"')
            {
                ++i;
                auto j = std::find_if(i, value.cend(), [](char c) { return c == '\"'; });
                if (j == value.cend())
                {
                    esink.push(error_tag(node.get_range(), "unmatched \""));
                    return boost::none;
                }

                arg.insert(arg.end(), i, j);
                ++j;
                i = j;
            }
            else if (is_ascii_whitespace(c))
                break;
            else
            {
                arg += c;
                ++i;
            }
        }

        split.push_back(arg);
    }

    if (split.empty())
    {
        esink.push(error_tag(node.get_range(), "empty command line"));
        return boost::none;
    }

    cmd_line data;
    data.executable = split[0];
    data.arguments.assign(split.begin() + 1, split.end());

    return data;
}

boost::optional<cmd_line> read_value_node_impl<cmd_line>::value_for_pseudo_identifier(pseudo_identifier_value_node const&)
{
    return boost::none;
}

const char* read_value_node_impl<command>::get_type_name()
{
    return "command";
}

bool read_value_node_impl<command>::can_read(value_node const& node)
{
    if (node.get_type() != value_node_type::struct_)
        return false;

    struct_node const& cnode = static_cast<struct_node const&>(node);
    return cnode.get_tag().get_text() == get_type_name();
}

boost::optional<command> read_value_node_impl<command>::read(value_node const& node, error_tag_sink& esink)
{
    assert(node.get_type() == value_node_type::struct_);

    struct_node const& cnode = static_cast<struct_node const&>(node);
    assert(cnode.get_tag().get_text() == get_type_name());

    std::multimap<std::string, property_node*> properties_by_name = make_properties_map(cnode, esink);

    boost::optional<cmd_line> cmd = extract_property_from_map<cmd_line>(cnode, properties_by_name, "cmd", esink);
    boost::optional<std::string> working_directory = extract_property_from_map<std::string>(cnode, properties_by_name, "working_directory", esink);
    if (!cmd || !working_directory)
        return boost::none;

    command data;

    data.cmd = *cmd;
    data.working_directory = *working_directory;

    return data;
}

boost::optional<command> read_value_node_impl<command>::value_for_pseudo_identifier(pseudo_identifier_value_node const&)
{
    return boost::none;
}

const char* read_value_node_impl<start_stop_task_data>::get_type_name()
{
    return "start_stop";
}

bool read_value_node_impl<start_stop_task_data>::can_read(value_node const& node)
{
    if (node.get_type() != value_node_type::struct_)
        return false;

    struct_node const& cnode = static_cast<struct_node const&>(node);
    return cnode.get_tag().get_text() == get_type_name();
}

boost::optional<start_stop_task_data> read_value_node_impl<start_stop_task_data>::read(value_node const& node, error_tag_sink& esink)
{
    assert(node.get_type() == value_node_type::struct_);

    struct_node const& cnode = static_cast<struct_node const&>(node);
    assert(cnode.get_tag().get_text() == get_type_name());

    std::multimap<std::string, property_node*> properties_by_name = make_properties_map(cnode, esink);

    start_stop_task_data data;

    boost::optional<command> start_cmd = extract_property_from_map<command>(cnode, properties_by_name, "start", esink);
    boost::optional<command> stop_cmd = extract_property_from_map<command>(cnode, properties_by_name, "stop", esink);
    if (!start_cmd || !stop_cmd)
        return boost::none;

    data.start = *start_cmd;
    data.stop  = *stop_cmd;

    return data;
}

boost::optional<null_task_data> read_value_node_impl<start_stop_task_data>::value_for_pseudo_identifier(pseudo_identifier_value_node const&)
{
    return boost::none;
}

const char* read_value_node_impl<control_task_data>::get_type_name()
{
    return "control";
}

bool read_value_node_impl<control_task_data>::can_read(value_node const& node)
{
    if (node.get_type() != value_node_type::struct_)
        return false;

    struct_node const& cnode = static_cast<struct_node const&>(node);
    return cnode.get_tag().get_text() == get_type_name();
}

boost::optional<control_task_data> read_value_node_impl<control_task_data>::read(value_node const& node, error_tag_sink& esink)
{
    assert(node.get_type() == value_node_type::struct_);

    struct_node const& cnode = static_cast<struct_node const&>(node);
    assert(cnode.get_tag().get_text() == get_type_name());

    std::multimap<std::string, property_node*> properties_by_name = make_properties_map(cnode, esink);

    control_task_data data;

    boost::optional<std::string> uds_filename = extract_property_from_map<std::string>(cnode, properties_by_name, "uds_filename", esink);
    if (!uds_filename)
        return boost::none;
    data.uds_filename = *uds_filename;

    return data;
}

boost::optional<control_task_data> read_value_node_impl<control_task_data>::value_for_pseudo_identifier(pseudo_identifier_value_node const&)
{
    return boost::none;
}

const char* read_value_node_impl<null_task_data>::get_type_name()
{
    return "null";
}

bool read_value_node_impl<null_task_data>::can_read(value_node const& node)
{
    if (node.get_type() != value_node_type::struct_)
        return false;

    struct_node const& cnode = static_cast<struct_node const&>(node);
    return cnode.get_tag().get_text() == get_type_name();
}

boost::optional<null_task_data> read_value_node_impl<null_task_data>::read(value_node const& node, error_tag_sink& esink)
{
    assert(node.get_type() == value_node_type::struct_);

    struct_node const& cnode = static_cast<struct_node const&>(node);
    assert(cnode.get_tag().get_text() == get_type_name());

    std::multimap<std::string, property_node*> properties_by_name = make_properties_map(cnode, esink);

    return null_task_data();
}

boost::optional<null_task_data> read_value_node_impl<null_task_data>::value_for_pseudo_identifier(pseudo_identifier_value_node const&)
{
    return boost::none;
}

const char* read_value_node_impl<task_data>::get_type_name()
{
    return "task";
}

bool read_value_node_impl<task_data>::can_read(value_node const& node)
{
    return read_value_node_impl<hostname_task_data>::can_read(node)
        || read_value_node_impl<start_stop_task_data>::can_read(node)
        || read_value_node_impl<control_task_data>::can_read(node)
        || read_value_node_impl<null_task_data>::can_read(node);
}

boost::optional<task_data> read_value_node_impl<task_data>::read(value_node const& node, error_tag_sink& esink)
{
    if (can_read_value_node<hostname_task_data>(node))
        return optional_cast<task_data>(read_value_node<hostname_task_data>(node, esink));
    else if (can_read_value_node<start_stop_task_data>(node))
        return optional_cast<task_data>(read_value_node<start_stop_task_data>(node, esink));
    else if (can_read_value_node<control_task_data>(node))
        return optional_cast<task_data>(read_value_node<control_task_data>(node, esink));
    else if (can_read_value_node<null_task_data>(node))
        return optional_cast<task_data>(read_value_node<null_task_data>(node, esink));
    else
    {
        assert(false);
        return boost::none;
    }
}

boost::optional<task_data> read_value_node_impl<task_data>::value_for_pseudo_identifier(pseudo_identifier_value_node const&)
{
    return boost::none;
}

boost::optional<task_data> read_task(task_node const& node, error_tag_sink& esink)
{
    if (!node.get_value())
        return boost::none;

    if (node.get_value()->get_type() == value_node_type::pseudo_identifier)
    {
        pseudo_identifier_value_node const& cvalue = static_cast<pseudo_identifier_value_node const&>(*node.get_value());

        std::stringstream ss;
        ss << "excepted value of type \"" << read_value_node_impl<task_data>::get_type_name() << "\", but identifier found";

        esink.push(error_tag(cvalue.get_token().get_range(), ss.str()));
        return read_value_node_impl<task_data>::value_for_pseudo_identifier(cvalue);
    }

    if (!can_read_value_node<task_data>(*node.get_value()))
    {
        std::stringstream ss;
        ss << "\"" << get_type_by_value_node(*node.get_value()) << "\" is not a valid type for task";

        esink.push(error_tag(node.get_value()->get_range(), ss.str()));
        return boost::none;
    }

    return read_value_node<task_data>(*node.get_value(), esink);
}
