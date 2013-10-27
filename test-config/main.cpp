#define BOOST_TEST_MODULE initd_config
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN

#include <fcntl.h>

#include <sstream>
#include <map>
#include <utility>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>

#include "file_descriptor.h"

#include "task_description.h"
#include "task_descriptions.h"

#include "read_value_node/read_task_description.h"

namespace fs = boost::filesystem;

boost::optional<std::vector<char> > read_entire_file_if_exists(fs::path const& filename)
{
    sysapi::file_descriptor fd = sysapi::file_descriptor::open_if_exists(filename.string(), O_RDONLY);
    if (fd.getfd() == -1)
        return boost::none;

    struct stat stat = fd.get_stat();

    std::vector<char> res(stat.st_size);

    fd.read(res.data(), res.size());

    return res;
}

void write_entire_file(fs::path const& filename, std::string const& data)
{
    auto fd = sysapi::file_descriptor::open(filename.string(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    fd.write(data.data(), data.size());
}

void dump(std::string const& data, std::ostream& out)
{
    out << "\"" << data << "\"";
}

void dump(bool data, std::ostream& out)
{
    out << (data ? "true" : "false");
}

void dump(mount_task_data const& data, std::ostream& out)
{
    out << "mount\n"
        << "{}\n";
}

void dump(hostname_task_data const& data, std::ostream& out)
{
    out << "sethostname\n"
        << "{\n"
        << "    hostname   = ";
    dump(data.hostname, out);
    out << "\n"
        << "    no_restore = ";
    dump(data.no_restore, out);
    out << "\n"
        << "}\n";
}

void dump(start_stop_task_data const& data, std::ostream& out)
{
    out << "start_stop\n"
        << "{}\n";
}
void dump(null_task_data const& data, std::ostream& out)
{
    out << "null\n"
        << "{}\n";
}

struct dump_visitor
{
    typedef void result_type;

    dump_visitor(std::ostream& out)
        : out(&out)
    {}

    template <typename T>
    void operator()(T const& d) const
    {
        dump(d, *out);
    }

    std::ostream* out;
};

void dump_tasks(task_descriptions const& descrs, std::ostream& out)
{
    for (task_description_sp const& t : descrs.get_all_tasks())
    {
        task_description const& d = *t;

        out << "task " << d.get_name() << " = ";
        apply_visitor(dump_visitor(out), d.get_data());
    }

    for (task_description_sp const& t : descrs.get_all_tasks())
    {
        task_description const& d = *t;

        if (!d.get_dependencies().empty())
        {
            out << d.get_name() << " depends ";

            auto i = d.get_dependencies().begin();
            out << (*i)->get_name();
            ++i;

            for (; i != d.get_dependencies().end(); ++i)
                out << ", " << (*i)->get_name();

            out << std::endl;
        }
    }
}

void run_test_on_file(fs::path const& filename)
{
    std::stringstream ss;

    task_descriptions descrs = read_descriptions(filename.string(), ss);

    dump_tasks(descrs, ss);

    std::string output = ss.str();
    std::vector<char> output_data{output.begin(), output.end()};

    fs::path gold_filename = filename;
    gold_filename.replace_extension(".gold");

    fs::path out_filename = filename;
    out_filename.replace_extension(".out");

    boost::optional<std::vector<char> > gold_data = read_entire_file_if_exists(gold_filename);

    if (!gold_data)
    {
        if (!output_data.empty())
        {
            std::stringstream ss;
            ss << "missing gold file \"" << gold_filename << "\"";
            BOOST_CHECK_MESSAGE(false, ss.str());
            write_entire_file(out_filename, output);
        }
        return;
    }

    if (output_data != *gold_data)
    {
        BOOST_CHECK_MESSAGE(false, "gold/output mismatch");
        write_entire_file(out_filename, output);
    }
}

bool init_function()
{
    for (fs::directory_iterator i = fs::directory_iterator("./config_tests/"), end; i != end; ++i)
        if (i->path().extension() == ".txt")
        {
            boost::unit_test::framework::master_test_suite().
                    add( boost::unit_test::make_test_case( boost::unit_test::callback0<>(boost::bind(&run_test_on_file, i->path())), "run_test_on_file(\"" + i->path().string() + "\")"));

        }

    return true;
}

int main( int argc, char* argv[] )
{
    return ::boost::unit_test::unit_test_main( &init_function, argc, argv );
}
