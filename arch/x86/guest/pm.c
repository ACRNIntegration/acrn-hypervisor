/*
 * Copyright (C) 2018 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <acrn_common.h>
#include <hv_lib.h>
#include <hv_arch.h>

int validate_pstate(struct vm *vm, uint64_t perf_ctl)
{
	struct cpu_px_data *px_data;
	int i, px_cnt;

	if (is_vm0(vm)) {
		return 0;
	}

	px_cnt = vm->pm.px_cnt;
	px_data = vm->pm.px_data;

	if (!px_cnt || !px_data) {
		return -1;
	}

	for (i = 0; i < px_cnt; i++) {
		if ((px_data + i)->control == (perf_ctl & 0xffff)) {
			return 0;
		}
	}

	return -1;
}

static void vm_setup_cpu_px(struct vm *vm)
{
	uint32_t px_data_size;

	vm->pm.px_cnt = 0;
	memset(vm->pm.px_data, 0, MAX_PSTATE * sizeof(struct cpu_px_data));

	if ((!boot_cpu_data.state_info.px_cnt)
		|| (!boot_cpu_data.state_info.px_data)) {
		return;
	}

	if (boot_cpu_data.state_info.px_cnt > MAX_PSTATE) {
		vm->pm.px_cnt = MAX_PSTATE;
	} else {
		vm->pm.px_cnt = boot_cpu_data.state_info.px_cnt;
	}

	px_data_size = vm->pm.px_cnt * sizeof(struct cpu_px_data);

	memcpy_s(vm->pm.px_data, px_data_size,
			boot_cpu_data.state_info.px_data, px_data_size);

}

void vm_setup_cpu_state(struct vm *vm)
{
	vm_setup_cpu_px(vm);
}
