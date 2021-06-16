#include <Python.h>

#include "litmus.h"

// Error object used for a failed liblitmus call
static PyObject *LiblitmusError;

/////////////////////////////////////////////////////////
// Functions that call into liblitmus                  //
/////////////////////////////////////////////////////////
static PyObject * get_wctime(PyObject *self, PyObject *args)
{
    double res = wctime();

    printf("Called wctime\n");

    return PyLong_FromLong(res);
}

static PyObject * get_litmus_clock(PyObject *self, PyObject *args)
{
    double res = litmus_clock();

    printf("Called litmus_clock\n");

    return PyLong_FromLong(res);
}

static PyObject * call_set_rt_task_param(PyObject *self, PyObject *args)
{
    // Parameters to parse out
    lt_t exec_cost;
    lt_t period;
    lt_t relative_deadline;
    lt_t phase;
    int early;

    // K: unsigned long long
    // p: boolean (int)
    if (!PyArg_ParseTuple(args, "KKKKp", &exec_cost, &period, &relative_deadline, &phase, &early))
    {
        return NULL;
    }

    // Build the rt_task parameter struct
    struct rt_task param;
    init_rt_task_param(&param);
    param.exec_cost = ms2ns(exec_cost);
    param.period = ms2ns(period);
    param.relative_deadline = ms2ns(relative_deadline);
    param.phase = ms2ns(phase);
    param.budget_policy = NO_ENFORCEMENT;
    if (early)
        param.release_policy = TASK_EARLY; /* early releasing */
    else
        param.release_policy = TASK_PERIODIC;
    param.cls = RT_CLASS_SOFT;
    param.priority = LITMUS_LOWEST_PRIORITY;
    // param.cpu = 1;

    // Now call set_rt_task_param with the built struct
    double res = set_rt_task_param(gettid(), &param);

    if (res != 0)
    {
        fprintf(stderr, "set_rt_task_param call failed\n");

        PyErr_SetString(LiblitmusError, "set_rt_task_param call failed\n");
        return NULL;
    }

    return PyLong_FromLong(res);
}

static PyObject * call_init_litmus(PyObject *self, PyObject *args)
{
    double res = init_litmus();

    if (res != 0)
    {
        fprintf(stderr, "init_litmus call failed\n");

        PyErr_SetString(LiblitmusError, "init_litmus call failed\n");
        return NULL;
    }

    return PyLong_FromLong(res);
}

static PyObject * set_task_mode_background(PyObject *self, PyObject *args)
{
    double res = task_mode(BACKGROUND_TASK);

    if (res != 0)
    {
        fprintf(stderr, "task_mode(BACKGROUND_TASK) call failed\n");

        PyErr_SetString(LiblitmusError, "task_mode(BACKGROUND_TASK) call failed\n");
        return NULL;
    }

    return PyLong_FromLong(res);
}

static PyObject * set_task_mode_litmusrt(PyObject *self, PyObject *args)
{
    double res = task_mode(LITMUS_RT_TASK);

    if (res != 0)
    {
        fprintf(stderr, "task_mode(LITMUS_RT_TASK) call failed\n");

        PyErr_SetString(LiblitmusError, "task_mode(LITMUS_RT_TASK) call failed\n");
        return NULL;
    }

    return PyLong_FromLong(res);
}

static PyObject * call_wait_for_ts_release(PyObject *self, PyObject *args)
{
    double res = wait_for_ts_release();

    if (res != 0)
    {
        fprintf(stderr, "wait_for_ts_release call failed\n");

        PyErr_SetString(LiblitmusError, "wait_for_ts_release call failed\n");
        return NULL;
    }

    return PyLong_FromLong(res);
}

static PyObject * call_sleep_next_period(PyObject *self, PyObject *args)
{
    double res = sleep_next_period();

    if (res != 0)
    {
        fprintf(stderr, "sleep_next_period call failed\n");

        PyErr_SetString(LiblitmusError, "sleep_next_period call failed\n");
        return NULL;
    }

    return PyLong_FromLong(res);
}

/////////////////////////////////////////////////////////
// All methods implemented in this wrapper             //
/////////////////////////////////////////////////////////
static PyMethodDef liblitmus_methods[] = {
    {
        "get_wctime",
        get_wctime,
        METH_VARARGS,
        "Obtain wall-clock time (in seconds)."
    },
    {
        "get_litmus_clock",
        get_litmus_clock,
        METH_VARARGS,
        "Get the current time used by the LITMUS^RT scheduler (nanoseconds)."
    },
    {
        "call_set_rt_task_param",
        call_set_rt_task_param,
        METH_VARARGS,
        "Set real-time task parameters for given process (assumes ms)."
    },
    {
        "call_init_litmus",
        call_init_litmus,
        METH_VARARGS,
        "Initialises real-time properties for the entire program."
    },
    {
        "set_task_mode_background",
        set_task_mode_background,
        METH_VARARGS,
        "Set the task mode for current thread to be BACKGROUND_TASK."
    },
    {
        "set_task_mode_litmusrt",
        set_task_mode_litmusrt,
        METH_VARARGS,
        "Set the task mode for current thread to be LITMUS_RT_TASK."
    },
    {
        "call_wait_for_ts_release",
        call_wait_for_ts_release,
        METH_VARARGS,
        "Wait until task master releases all real-time tasks."
    },
    {
        "call_sleep_next_period",
        call_sleep_next_period,
        METH_VARARGS,
        "Sleep until next period."
    },
    {NULL, NULL, 0, NULL} /* must be last entry */
};

/////////////////////////////////////////////////////////
// The liblitmus wrapper module definition             //
/////////////////////////////////////////////////////////
static struct PyModuleDef liblitmus_module = {
    PyModuleDef_HEAD_INIT,
    "liblitmus",
    "A Python wrapper for liblitmus.",
    -1,
    liblitmus_methods,
    NULL,
    NULL,
    NULL,
    NULL,
};

/////////////////////////////////////////////////////////
// The liblitmus wrapper module initialization         //
/////////////////////////////////////////////////////////
PyMODINIT_FUNC PyInit_liblitmus(void)
{
    PyObject *m;

    m = PyModule_Create(&liblitmus_module);
    if (m == NULL)
    {
        return NULL;
    }

    LiblitmusError = PyErr_NewException("liblitmus.error", NULL, NULL);
    Py_INCREF(LiblitmusError);
    PyModule_AddObject(m, "error", LiblitmusError);

    return m;
}