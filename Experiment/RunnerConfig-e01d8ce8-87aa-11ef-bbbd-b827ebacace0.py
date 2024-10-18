from EventManager.Models.RunnerEvents import RunnerEvents
from EventManager.EventSubscriptionController import EventSubscriptionController
from ConfigValidator.Config.Models.RunTableModel import RunTableModel
from ConfigValidator.Config.Models.FactorModel import FactorModel
from ConfigValidator.Config.Models.RunnerContext import RunnerContext
from ConfigValidator.Config.Models.OperationType import OperationType
from ExtendedTyping.Typing import SupportsStr
from ProgressManager.Output.OutputProcedure import OutputProcedure as output
import pandas as pd

from typing import Dict, List, Any, Optional
from pathlib import Path
from os.path import dirname, realpath
import subprocess
import os
import time  # Import time for measuring durations
import logging  # Import logging for writing to log files
import shlex
import os

class RunnerConfig:
    ROOT_DIR = Path(dirname(realpath(__file__)))

    # ================================ USER SPECIFIC CONFIG ================================
    """The name of the experiment."""
    name: str = "new_runner_experiment_1"

    """Output path defaults to the config file's path, inside the folder 'experiments'"""
    results_output_path: Path = ROOT_DIR / "experiments"

    """Experiment operation type."""
    operation_type: OperationType = OperationType.AUTO

    """The time Experiment Runner will wait after a run completes."""
    time_between_runs_in_ms: int = 1000

    def __init__(self):
        """Executes immediately after program start, on config load"""
        EventSubscriptionController.subscribe_to_multiple_events(
            [
                (RunnerEvents.BEFORE_EXPERIMENT, self.before_experiment),
                (RunnerEvents.BEFORE_RUN, self.before_run),
                (RunnerEvents.START_RUN, self.start_run),
                (RunnerEvents.START_MEASUREMENT, self.start_measurement),
                (RunnerEvents.INTERACT, self.interact),
                (RunnerEvents.STOP_MEASUREMENT, self.stop_measurement),
                (RunnerEvents.STOP_RUN, self.stop_run),
                (RunnerEvents.POPULATE_RUN_DATA, self.populate_run_data),
                (RunnerEvents.AFTER_EXPERIMENT, self.after_experiment),
            ]
        )
        self.run_table_model = None  # Initialized later
        self.total_start_time = None  # For total experiment time tracking
        self.run_start_time = None  # For tracking individual run times
        self.run_time = None

        # Initialize logging to file
        logging.basicConfig(
            filename="experiment_log.txt",
            level=logging.INFO,
            format="%(asctime)s - %(message)s",
            filemode="w",
        )
        logging.info("Experiment started.")
        output.console_log("Custom config loaded")

    def create_run_table_model(self) -> RunTableModel:
        """Create and return the run_table model."""
        llm_factor = FactorModel("llm", ["ChatGPT", "Claude", "Gemeni", "Llama", "Mistral"])
        algorithm_factor = FactorModel("algorithm", ["BB", "BFA", "SWA"])
        self.run_table_model = RunTableModel(
            factors=[llm_factor, algorithm_factor],
            exclude_variations=[],
            repetitions= 2,
            data_columns=["cpu_freq_0", "cpu_freq_1", "cpu_freq_2", "cpu_freq_3",
                          "cpu_usage_0", "cpu_usage_1","cpu_usage_2", "cpu_usage_3",
                          "total_memory", "used_memory", "total_swap", "used_swap",
                          "dram_energy", "package_energy", "pp0_energy", "pp1_energy",
                          "run_time"],
        )
        return self.run_table_model

    def before_experiment(self) -> None:
        """Activity before starting the experiment."""
        output.console_log("Config.before_experiment() called!")
        self.total_start_time = time.time()  # Start the total time tracker
        logging.info("Experiment started.")

    def before_run(self) -> None:
        """Activity before starting a run."""
        output.console_log("Config.before_run() called!")
        self.run_start_time = time.time()  # Start the individual run time tracker

    def start_run(self, context: RunnerContext) -> None:
        # """Perform any activity required for starting the run here."""
        # algorithm = context.run_variation["algorithm"]
        # llm = context.run_variation["llm"]
        # current_path = os.getcwd()
        # output.console_log(f"Current working directory: {current_path}")
        #
        # # Use ROOT_DIR to build the path to 'sol'
        # cpp_file = os.path.join(self.ROOT_DIR, f"{algorithm}/{llm}/sol.cpp")
        # executable_file = os.path.join(self.ROOT_DIR, f"{algorithm}/{llm}/sol")
        #
        # if not os.path.exists(cpp_file):
        #     output.console_log(f"File not found: {cpp_file}")
        #     logging.error(f"File not found: {cpp_file}")
        #     return
        #
        # # Create output directory if it doesn't exist
        # output_dir = os.path.dirname(executable_file)
        # if not os.path.exists(output_dir):
        #     os.makedirs(output_dir)
        #
        # # Measure compilation time
        # compile_start_time = time.time()
        # try:
        #     compile_process = subprocess.run(
        #         ["g++", "-o", executable_file, cpp_file],
        #         stdout=subprocess.PIPE,
        #         stderr=subprocess.PIPE,
        #         check=True,
        #     )
        #     compile_end_time = time.time()
        #     compile_duration = compile_end_time - compile_start_time
        #     output.console_log(
        #         f"Compilation successful! Time taken: {compile_duration:.2f} seconds"
        #     )
        #     logging.info(
        #         f"Compilation successful! Time taken: {compile_duration:.2f} seconds"
        #     )
        # except subprocess.CalledProcessError as compile_error:
        #     output.console_log(f"Compilation failed: {compile_error.stderr.decode()}")
        #     logging.error(f"Compilation failed: {compile_error.stderr.decode()}")
        #     return
        #
        # if not os.path.exists(executable_file):
        #     output.console_log(f"Compiled executable not found: {executable_file}")
        #     logging.error(f"Compiled executable not found: {executable_file}")
        #     return
        #
        # output.console_log(f"Executable file found: {executable_file}")
        #
        # # Execute the compiled file
        # try:
        #     self.target = subprocess.Popen(
        #         [executable_file],
        #         stdout=subprocess.PIPE,
        #         stderr=subprocess.PIPE,
        #         cwd=self.ROOT_DIR,
        #     )
        #     output.console_log("Execution started successfully!")
        #     logging.info("Execution started successfully!")
        # except FileNotFoundError as exec_error:
        #     output.console_log(f"Execution failed: {exec_error}")
        #     logging.error(f"Execution failed: {exec_error}")
        #     return

        output.console_log("Config.start_run() called!")

    def start_measurement(self, context: RunnerContext) -> None:
        """Perform any activity required for starting measurements."""
        llm = context.run_variation["llm"]
        algorithm = context.run_variation["algorithm"]

        output.console_log(f'/home/mark/greenlab/experiment-runner/examples/Green-Lab-LLM/Experiment/{algorithm}/{llm}/sol')

        profiler_cmd = f'energibridge \
                        --interval 20 \
                        --max-execution 20 \
                        --output {context.run_dir / "energibridge.csv"} \
                        --summary \
                        /home/mark/greenlab/experiment-runner/examples/Green-Lab-LLM/Experiment/{algorithm}/{llm}/sol'

        output.console_log("Config.start_measurement() called!")
        energibridge_log = open(f'{context.run_dir}/energibridge.log', 'w')
        output.console_log(f'{algorithm}/{llm}/sol')
        self.profiler = subprocess.Popen(shlex.split(profiler_cmd), stdout=energibridge_log)

    def interact(self, context: RunnerContext) -> None:
        """Perform any interaction with the running target system here."""
        output.console_log("Config.interact() called!")

    def stop_measurement(self, context: RunnerContext) -> None:
        """Perform any activity here required for stopping measurements."""
        self.profiler.wait()
        output.console_log("Config.stop_measurement called!")

    def stop_run(self, context: RunnerContext) -> None:
        """Perform any activity here required for stopping the run."""
        run_end_time = time.time()
        run_duration = run_end_time - self.run_start_time
        self.run_time = run_duration
        output.console_log(
            f"Config.stop_run() called! Run time: {run_duration:.2f} seconds"
        )
        logging.info(f"Run ended. Run time: {run_duration:.2f} seconds")

    def populate_run_data(self, context: RunnerContext) -> Optional[Dict[str, SupportsStr]]:
        output.console_log("Config.populate_run_data() called!")
        """Parse and process any measurement data here."""

        # Read the powerjoular CSV file
        df = pd.read_csv(context.run_dir / f"energibridge.csv")

        run_data = {
            'dram_energy': round(df['DRAM_ENERGY (J)'].sum(), 3),
            'package_energy': round(df['PACKAGE_ENERGY (J)'].sum(), 3),
            'pp0_energy': round(df['PP0_ENERGY (J)'].sum(), 3),
            'pp1_energy': round(df['PP1_ENERGY (J)'].sum(), 3),
            'cpu_freq_0': round(df['CPU_FREQUENCY_0'].mean(), 2),
            'cpu_freq_1': round(df['CPU_FREQUENCY_1'].mean(), 2),
            'cpu_freq_2': round(df['CPU_FREQUENCY_2'].mean(), 2),
            'cpu_freq_3': round(df['CPU_FREQUENCY_3'].mean(), 2),
            'cpu_usage_0': round(df['CPU_USAGE_0'].mean(), 2),
            'cpu_usage_1': round(df['CPU_USAGE_1'].mean(), 2),
            'cpu_usage_2': round(df['CPU_USAGE_2'].mean(), 2),
            'cpu_usage_3': round(df['CPU_USAGE_3'].mean(), 2),
            'total_memory': round(df['TOTAL_MEMORY'].mean(), 2),
            'used_memory': round(df['USED_MEMORY'].mean(), 2),
            'total_swap': round(df['TOTAL_SWAP'].mean(), 2),
            'used_swap': round(df['USED_SWAP'].mean(), 2),
            'run_time': self.run_time
        }
        return run_data

    def after_experiment(self) -> None:
        """Perform any activity required after stopping the experiment."""
        total_end_time = time.time()
        total_duration = total_end_time - self.total_start_time
        output.console_log(
            f"Config.after_experiment() called! Total experiment time: {total_duration:.2f} seconds"
        )
        logging.info(f"Experiment completed. Total time: {total_duration:.2f} seconds")

    # ================================ DO NOT ALTER BELOW THIS LINE ================================
    experiment_path: Path = None
