# This file is part of the pCloud Console Client.
#
# (c) 2021 Serghei Iakovlev <egrep@protonmail.ch>
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.

include(TargetArch)

function(get_target_architecture output_var)
  target_architecture(local_output_var)
  set(${output_var} ${local_output_var} PARENT_SCOPE)
endfunction()
