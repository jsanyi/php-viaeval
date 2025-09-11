/* viaeval extension for PHP */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_viaeval.h"

static void (*zend_old_execute_ex)(zend_execute_data *execute_data) = NULL;

static int viaeval_is_exception_file(const char *filename);

ZEND_DECLARE_MODULE_GLOBALS(viaeval)
static PHP_GINIT_FUNCTION(viaeval);

PHP_INI_BEGIN()
STD_PHP_INI_ENTRY("viaeval.disable_eval", "0", PHP_INI_PERDIR | PHP_INI_SYSTEM, OnUpdateBool, disable_eval, zend_viaeval_globals, viaeval_globals)
STD_PHP_INI_ENTRY("viaeval.monitor_eval", "0", PHP_INI_PERDIR | PHP_INI_SYSTEM, OnUpdateBool, monitor_eval, zend_viaeval_globals, viaeval_globals)
STD_PHP_INI_ENTRY("viaeval.eval_exclude", "", PHP_INI_PERDIR | PHP_INI_SYSTEM, OnUpdateString, eval_exclude, zend_viaeval_globals, viaeval_globals)
PHP_INI_END()

#if defined(COMPILE_DL_VIAEVAL) && PHP_VERSION_ID >= 70000
#if defined(ZTS)
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(viaeval)
#endif

static PHP_GINIT_FUNCTION(viaeval)
{
#if defined(ZTS) && defined(COMPILE_DL_VIAEVAL) && PHP_VERSION_ID >= 70000
    ZEND_TSRMLS_CACHE_UPDATE();
#endif
    viaeval_globals->disable_eval = 0;
    viaeval_globals->monitor_eval = 0;
    viaeval_globals->eval_exclude = NULL;
}

static int viaeval_is_exception_file(const char *filename)
{
    if (!filename || !VIAEVAL_G(eval_exclusions_ht.nNumOfElements))
        return 0;
    zend_string *key;
    ZEND_HASH_FOREACH_STR_KEY(&VIAEVAL_G(eval_exclusions_ht), key)
    {
        size_t filename_len = strlen(filename);
        size_t key_len = ZSTR_LEN(key);

        if (key_len > 0 && (ZSTR_VAL(key)[key_len - 1] == '/' || ZSTR_VAL(key)[key_len - 1] == '\\'))
        {
            const char *dir_pos = NULL;
            for (const char *p = filename; (p = strstr(p, ZSTR_VAL(key))); p++)
            {
                dir_pos = p;
            }
            if (dir_pos)
            {
                if (dir_pos + key_len < filename + filename_len)
                {
                    return 1;
                }
            }
        }
        else if (filename_len >= key_len)
        {
            const char *filename_end = filename + filename_len - key_len;
            if (strcmp(filename_end, ZSTR_VAL(key)) == 0)
            {
                if (filename_len == key_len ||
                    filename_end[-1] == '/' ||
                    filename_end[-1] == '\\')
                {
                    return 1;
                }
            }
        }
    } ZEND_HASH_FOREACH_END();
    return 0;
}

void viaeval_execute_ex(zend_execute_data *execute_data TSRMLS_DC)
{
    const char *current_filename = zend_get_executed_filename();

#if PHP_VERSION_ID < 70000
    const zend_op_array *op_array = execute_data->op_array;
#elif PHP_VERSION_ID < 80000
    const zend_op_array *op_array = &execute_data->func->op_array;
#endif

    if (!VIAEVAL_G(disable_eval))
    {
#if PHP_VERSION_ID >= 80000
        if (execute_data->opline && (ZEND_INCLUDE_OR_EVAL == execute_data->opline->opcode) && (ZEND_EVAL == execute_data->opline->extended_value))
#else
        if (op_array->type == ZEND_EVAL_CODE)
#endif
        {
            if (VIAEVAL_G(monitor_eval) && !viaeval_is_exception_file(current_filename))
            {
                php_error(E_WARNING, "[viaeval] eval() was called!");
            }
        }
        zend_old_execute_ex(execute_data TSRMLS_DC);
        return;
    }

#if PHP_VERSION_ID >= 80000
    if (execute_data->opline && (ZEND_INCLUDE_OR_EVAL == execute_data->opline->opcode) && (ZEND_EVAL == execute_data->opline->extended_value) && !viaeval_is_exception_file(current_filename))
#else
    if (op_array->type == ZEND_EVAL_CODE && !viaeval_is_exception_file(current_filename))
#endif
    {
#ifdef HIDE_PRESENCE
        zend_error(E_PARSE, PHP_VIAEVAL_MSG);
#else
        zend_error(E_ERROR, PHP_VIAEVAL_MSG);
#endif
        zend_bailout();
    }

    zend_old_execute_ex(execute_data TSRMLS_DC);
}

PHP_MINIT_FUNCTION(viaeval)
{
    REGISTER_INI_ENTRIES();
    zend_old_execute_ex = zend_execute_ex;
    zend_execute_ex = viaeval_execute_ex;

    if (VIAEVAL_G(eval_exclude) && *VIAEVAL_G(eval_exclude))
    {
        zend_hash_init(&VIAEVAL_G(eval_exclusions_ht), 8, NULL, NULL, 1);
        char *list = estrdup(VIAEVAL_G(eval_exclude));
        char *token = strtok(list, ",");
        while (token)
        {
            while (*token == ' ')
                token++;
            char *end = token + strlen(token) - 1;
            while (end > token && *end == ' ')
                *end-- = '\0';
            zend_hash_str_add_empty_element(&VIAEVAL_G(eval_exclusions_ht), token, strlen(token));
            token = strtok(NULL, ",");
        }
        efree(list);
    }

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(viaeval)
{
    UNREGISTER_INI_ENTRIES();
    zend_execute_ex = zend_old_execute_ex;
    zend_hash_destroy(&VIAEVAL_G(eval_exclusions_ht));
    return SUCCESS;
}

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(viaeval)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "ViaEval", "Loaded");
    php_info_print_table_row(2, "Message", PHP_VIAEVAL_MSG);
    php_info_print_table_row(2, "Version", PHP_VIAEVAL_VERSION);
    php_info_print_table_row(2, "viaeval.disable_eval", INI_BOOL("viaeval.disable_eval") ? "On" : "Off");
    php_info_print_table_row(2, "viaeval.monitor_eval", INI_BOOL("viaeval.monitor_eval") ? "On" : "Off");

    zend_string *key;
    size_t total_len = 0;
    size_t count = 0;

    ZEND_HASH_FOREACH_STR_KEY(&VIAEVAL_G(eval_exclusions_ht), key) {
        total_len += ZSTR_LEN(key) + 2;
        count++;
    } ZEND_HASH_FOREACH_END();

    if (count > 0) {
        char *formatted = emalloc(total_len + 1);
        char *dst = formatted;
        size_t i = 0;
        ZEND_HASH_FOREACH_STR_KEY(&VIAEVAL_G(eval_exclusions_ht), key) {
            size_t key_len = ZSTR_LEN(key);
            memcpy(dst, ZSTR_VAL(key), key_len);
            dst += key_len;
            if (++i < count) {
                memcpy(dst, ", ", 2);
                dst += 2;
            }
        } ZEND_HASH_FOREACH_END();
        *dst = '\0';
        php_info_print_table_row(2, "viaeval.eval_exclude", formatted);
        efree(formatted);
    } else {
        php_info_print_table_row(2, "viaeval.eval_exclude", "");
    }

    php_info_print_table_end();
}
/* }}} */

PHP_RINIT_FUNCTION(viaeval)
{
#if defined(ZTS) && defined(COMPILE_DL_DISEVAL)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif
    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(viaeval)
{
    return SUCCESS;
}

/* {{{ viaeval_module_entry */
zend_module_entry viaeval_module_entry = {
    STANDARD_MODULE_HEADER,
    "viaeval",
    NULL,
    PHP_MINIT(viaeval),
    PHP_MSHUTDOWN(viaeval),
    PHP_RINIT(viaeval),
    PHP_RSHUTDOWN(viaeval),
    PHP_MINFO(viaeval),
    PHP_VIAEVAL_VERSION,
    PHP_MODULE_GLOBALS(viaeval),
    PHP_GINIT(viaeval),
    NULL,
    NULL,
    STANDARD_MODULE_PROPERTIES_EX};
/* }}} */
