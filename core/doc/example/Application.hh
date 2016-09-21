namespace xtd {
namespace {


class MyApp : public Application
{
public:
  MyApp(void) :
    Application()
  {
    addOption('i', "input-file",
              argument::mandatory,
              requirement::mandatory,
              "process given file",
              Application::bindFile(m_inputFile, true));
  }

private:
  int process(void)
  {
    std::cout << "given readable file : " << m_inputFile << std::endl;

    // do some stuff
    return (l_success) ? 0 : 1;
  }

private:
  std::string m_inputFile;
};


int main(int p_argc, char** p_argv)
{
  MyApp l_app;
  return l_app.execute(p_argc, p_argv);
}

}
