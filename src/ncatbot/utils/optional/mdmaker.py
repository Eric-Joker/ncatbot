# 发送图片 MarkDown 文件

import os
import tempfile

from pygments.formatters import HtmlFormatter

from ncatbot.utils.logger import get_log

_log = get_log("utils")


def read_file(file_path) -> any:
    with open(file_path, "r", encoding="utf-8") as f:
        return f.read()


def get_chrome_path():
    raise NotImplementedError


def markdown_to_html(md_content, external_css_urls=None, custom_css=""):
    raise NotImplementedError


async def html_to_png(html_content, output_png, chrome_executable=None):
    raise NotImplementedError


async def md_maker(md_content):
    """
    将 Markdown 文本转换为 HTML，并生成 PNG 图片。
    :param md_content: Markdown 文本内容
    :return: 生成的 PNG 图片路径
    """
    current_path = os.path.dirname(os.path.abspath(__file__))
    external_css = read_file(os.path.join(current_path, "template/external.css"))
    highlight_css = HtmlFormatter().get_style_defs(".codehilite")
    custom_css = f"""
/* 基本重置与布局 */
html, body {{
    margin: 0;
    padding: 0;
    background-color: #f8f9fa;
    font-family: 'Helvetica Neue', Arial, sans-serif;  /* 现代化字体 */
}}
body {{
    padding: 20px;
}}

/* container 控制宽度 */
.container {{
    max-width: 960px;
    margin: auto;
}}

/* 自动换行处理，防止代码块超出页面宽度 */
pre code {{
    white-space: pre-wrap;
    word-break: break-all;
    font-family: 'Fira Code', 'Courier New', monospace;  /* 代码字体 */
}}

/* Pygments 代码高亮样式 */
{highlight_css}

/* 补充代码块背景和内边距（保证背景模板显示） */
.codehilite {{
    background: #f0f0f0;  /* 浅灰色背景 */
    color: #000000;
    padding: 1em;
    border-radius: 5px;
    overflow-x: auto;
    font-family: 'Fira Code', 'Courier New', monospace;  /* 代码字体 */
}}
pre.codehilite {{
    background: #f0f0f0;  /* 浅灰色背景 */
    color: #000000;
    padding: 1em;
    border-radius: 5px;
    overflow-x: auto;
    font-family: 'Fira Code', 'Courier New', monospace;  /* 代码字体 */
}}
/* 表格样式 */
table, th, td {{
    border: 1px solid #dee2e6;
    border-collapse: collapse;
}}
th, td {{
    padding: 0.75em;
    text-align: left;
}}
/* 针对超长文本、表格等设置 */
table {{
    width: 100%;
    margin-bottom: 1em;
}}
"""
    html_content = markdown_to_html(
        md_content, external_css_urls=external_css, custom_css=custom_css
    )
    chrome_path = get_chrome_path()
    if chrome_path is None:
        _log.info("未在注册表中找到 Chrome 浏览器路径，尝试自动安装Chromium")
        # raise Exception("未找到 Chrome 浏览器路径")
    else:
        _log.debug(f"Chrome 路径：{chrome_path}")
    output_png = os.path.join(tempfile.gettempdir(), "markdown.png")
    await html_to_png(html_content, output_png, chrome_path)
    return output_png
