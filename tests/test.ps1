$env:Path += (";" + $PSScriptRoot + "\..\build")

$TEST_DIR = $PSScriptRoot + "\..\lit-test"
New-Item -Path $TEST_DIR -ItemType Directory

Set-Location $TEST_DIR

Write-Output "== Initializing repository"
lit init

Write-Output "== Creating the first commit"
$content = @'
This is the first line of the first file. 🚀
This is the second line of the first file.
'@
$content | Out-File ".\file1" -Append

# Let's check the status. Should look something like this:
# Changes:
#   Add file1
lit status

# We expect the first commit to be identified as r0.
lit commit "Add file1"

Write-Output "== Creating more commits"
"A third line is added to the first file." | Out-File ".\file1" -Append

# Let's check the status again, just to be sure.
lit status

# This one would be r1.
lit commit "Extend file1"

"A forth line is added." | Out-File ".\file1" -Append

# This should be r2.
lit commit "Extend file1 even further"

Write-Output "== Displaying graph"

# o ← r2 Extend file1 even further
# o   r1 Extend file1
# o   r0 Add file1
#lit log

Write-Output "== Inspecting r0"
lit show r0

Write-Output "== Switching to r0"
lit checkout r0

# Checking the file content.
@'
This is the first line of the first file. 🚀
This is the second line of the first file.
'@ | diff.exe -s "${TEST_DIR}\file1" -

Write-Output "== Switching back to r2"
lit checkout r2

# Checking the file content again.
@'
This is the first line of the first file. 🚀
This is the second line of the first file.
A third line is added to the first file.
A forth line is added.
'@ | diff.exe -s "${TEST_DIR}\file1" -

Write-Output "== Adding and discarding changes"
"This fifth line should be gone in an instant." | Out-File ".\file1" -Append
lit checkout

# Let's confirm.
@'
This is the first line of the first file. 🚀
This is the second line of the first file.
A third line is added to the first file.
A forth line is added.
'@ | diff.exe -s "${TEST_DIR}\file1" -

Write-Output "== Creating another branch"
lit checkout r0

New-Item -Path ".\subfolder" -ItemType Directory

$content = @'
This is the first line of the second file.
And another line in the second file.
'@
$content | Out-File ".\subfolder\file2"

# This should be r3.
lit commit "Add file2"

# o   ← r3 Add file2
# │ o   r2 Extend file1 even further
# │ o   r1 Extend file1
# o─┘   r0 Add file1
#lit log

Write-Output "== Going back"
lit checkout r2

# file2 should be gone.
if (Test-Path ".\subfolder\file2" -PathType Leaf) {
  throw "Error: file2 should not exist!"
}

Write-Output "== Merging (no conflict)"

# This creates a merge commit r4.
lit merge r3

# file2 should now be present.
@'
This is the first line of the second file.
And another line in the second file.
'@ | diff.exe -s "${TEST_DIR}\subfolder\file2" -

# o─┐ ← r4 Merge r3 into r2
# │ o   r3 Add file2
# o │   r2 Extend file1 even further
# o │   r1 Extend file1
# o─┘   r0 Add file1
#lit log

Write-Output "== Setting up a conflict"
"Fifth line on top of r4." | Out-File ".\file1" -Append
lit commit "Extend file1 one way" # r5

lit checkout r3
"Third line on top of r3." | Out-File ".\file1" -Append
lit commit "Extend file1 another way" # r6

#   o ← r6 Extend file1 another way
# o │   r5 Extend file1 one way
# o─┤   r4 Merge r3 into r2
# │ o   r3 Add file2
# o │   r2 Extend file1 even further
# o │   r1 Extend file1
# o─┘   r0 Add file1
#lit log

# Going back and merging.
lit checkout r5
lit merge r6

# Let's check all file versions:
#   Current commit
@'
This is the first line of the first file. 🚀
This is the second line of the first file.
A third line is added to the first file.
A forth line is added.
Fifth line on top of r4.
'@ | diff.exe -s "${TEST_DIR}\file1" -

#   other branch
@'
This is the first line of the first file. 🚀
This is the second line of the first file.
Third line on top of r3.
'@ | diff.exe -s "${TEST_DIR}\file1.r6" -

#  common base
@'
This is the first line of the first file. 🚀
This is the second line of the first file.
'@ | diff.exe -s "${TEST_DIR}\file1.r3" -

# Let's simulate some conflict resolution.
"Sixth line added during merge conflict." | Out-File ".\file1" -Append

# Before continuing we need to cleanup the leftover files.
Remove-Item -Force file1.r6
Remove-Item -Force file1.r3

lit commit "Merge r6 into r5" # r7

# Let's verify the final result.

@'
This is the first line of the first file. 🚀
This is the second line of the first file.
A third line is added to the first file.
A forth line is added.
Fifth line on top of r4.
Sixth line added during merge conflict.
'@ | diff.exe -s "${TEST_DIR}\file1" -

# o─┐ ← r7 Merge r6 into r5
# │ o   r6 Extend file1 another way
# o │   r5 Extend file1 one way
# o─┤   r4 Merge r3 into r2
# │ o   r3 Add file2
# o │   r2 Extend file1 even further
# o │   r1 Extend file1
# o─┘   r0 Add file1
#lit log

Write-Output "== Cleanup"
Set-Location ..
Remove-Item -Recurse -Force $TEST_DIR