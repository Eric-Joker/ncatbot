#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace fs = std::filesystem;

std::string pythonVersion = "3.12.9";

std::string getUserInput(const std::vector<std::string>& validInputs, const std::string& prompt, const std::string& defaultValue = "") {
    std::string userInput;
    bool isValid = false;

    // ��ʾ�û�����
    std::cout << prompt;
    std::cin >> userInput;

    // ��������Ƿ�ƥ��
    for (const auto& input : validInputs) {
        if (userInput == input) {
            isValid = true;
            break;
        }
    }

    if (isValid) {
        return userInput;
    } else {
        // �������Ĭ��ֵ�ҵ�һ��������Ч������Ĭ��ֵ
        if (!defaultValue.empty()) {
            return defaultValue;
        } else {
            // ��ʾ�û���������
            while (true) {
                std::cout << "��Ч���룬����������: ";
                std::cin >> userInput;

                isValid = false;
                for (const auto& input : validInputs) {
                    if (userInput == input) {
                        isValid = true;
                        break;
                    }
                }

                if (isValid) {
                    return userInput;
                }
            }
        }
    }
}

void createDirectory(const std::string& path) {
    if (fs::exists(path)) {
        std::cerr << "�Ѵ��� ncatbot ·��: " << path << std::endl;
        std::cerr << "���ֶ�ɾ����Ŀ¼������" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (!fs::create_directory(path)) {
        std::cerr << "Failed to create directory: " << path << std::endl;
        std::cerr << "����һ���ڲ�����, ����ϵ����Ա\n";
        exit(EXIT_FAILURE);
    }
}

void installPythonWindows(const std::string& installDir) {
    std::string pythonUrl = "https://www.python.org/ftp/python/" + pythonVersion + "/python-" + pythonVersion + "-amd64.exe";
    std::string pythonExePath = installDir + "/python_installer.exe";
    std::string pythonInstallPath = installDir + "/Python";

    // ����Ŀ��Ŀ¼
    fs::create_directories(installDir);

    // ���ذ�װ��
    system(("powershell -Command \"[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; "
            "Invoke-WebRequest -Uri '" + pythonUrl + "' -OutFile '" + pythonExePath + "'\"").c_str());

    // ��Ĭ��װ����
    std::string installCmd = "\"" + pythonExePath + "\" /quiet InstallAllUsers=0 ";
    installCmd += "TargetDir=\"" + pythonInstallPath + "\" ";
    installCmd += "PrependPath=0 AssociateFiles=0 Shortcuts=0 Include_launcher=0";

    // ִ�а�װ
    system(installCmd.c_str());

    // ����װ��
    // fs::remove(pythonExePath);

    // ��װpip
    // std::string pythonExe = pythonInstallPath + "/python.exe";
    // system(("\"" + pythonExe + "\" -m ensurepip --upgrade --default-pip").c_str());
}

void installPythonLinux(const std::string& installDir) {
    std::string pythonTarUrl = "https://www.python.org/ftp/python/" + pythonVersion + "/Python-" + pythonVersion + ".tar.xz";
    std::string pythonTarPath = installDir + "/Python-" + pythonVersion + ".tar.xz";
    std::string pythonBuildPath = installDir + "/Python-" + pythonVersion;
    std::string pythonInstallPath = installDir + "/python";

    // ��װ��������
    system("sudo apt-get update > /dev/null 2>&1");
    system("sudo apt-get install -y --no-install-recommends build-essential zlib1g-dev libncurses5-dev "
           "libgdbm-dev libnss3-dev libssl-dev libreadline-dev libffi-dev libsqlite3-dev libbz2-dev > /dev/null 2>&1");

    // ����Դ��
    system(("curl -sSL -o '" + pythonTarPath + "' '" + pythonTarUrl + "'").c_str());

    // ��ѹԴ��
    system(("tar -xf '" + pythonTarPath + "' -C '" + installDir + "'").c_str());

    // ���ñ���ѡ��
    std::string configureCmd = "cd '" + pythonBuildPath + "' && ";
    configureCmd += "./configure --prefix='" + pythonInstallPath + "' ";
    configureCmd += "--enable-optimizations --with-ensurepip=install --enable-loadable-sqlite-extensions > /dev/null 2>&1";

    // ���밲װ
    system(configureCmd.c_str());
    system(("cd '" + pythonBuildPath + "' && make -j$(nproc) > /dev/null 2>&1 && make altinstall > /dev/null 2>&1").c_str());

    // ��֤��װ
    // std::string pythonExe = pythonInstallPath + "/bin/python3";
    // system(("\"" + pythonExe + "\" -m pip install --upgrade pip > /dev/null 2>&1"));

    // ������ʱ�ļ�
    fs::remove(pythonTarPath);
    fs::remove_all(pythonBuildPath);
}

void installPython(const std::string& installDir) {
    std::cout << "���ǽ��ڵ�ǰ����Ŀ¼�°�װ������ Python ����, �����޸�ϵͳ����, ��ȷ��:\n";
    std::string confirm = getUserInput({"y", "n"}, "�Ƿ�װ Python ����? (y/n): ");
    if(confirm == "n") {
        std::cout << "��ȡ����װ\n";
        exit(0);
    }
    #ifdef _WIN32
    installPythonWindows(installDir);
    #else
    installPythonLinux(installDir);
    #endif
}

int main(int argc, char* argv[]) {
    const std::string ncatbotDir = "ncatbot";
    const std::string pythonDir = ncatbotDir + "/python";

    // Create ncatbot directory
    createDirectory(ncatbotDir);

    // Install Python
    installPython(ncatbotDir);

    std::cout << "Python installed in " << pythonDir << std::endl;

    return 0;
}
